#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

#include <functional>
#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <mutex>
#include <thread>
#include <bitset>
#include <cmath>

using namespace std;

//ToDo: Flesh this class out
struct POD {};
struct Behaviour {};
struct System {};

template <unsigned int size = 1, unsigned int alignment = 1, unsigned int count = 1>
struct ClusterableWithBuffer
{
	alignas(alignment) std::byte buffer[count][size];

	ClusterableWithBuffer() noexcept
	{
		for(auto i = 0; i < count; ++i)
		{
			std::memset(&buffer[i], 0, size);
		}		
	}
};

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

struct lock_free_task
{
	atomic_flag isDone = ATOMIC_FLAG_INIT;
	std::function<void()> task = nullptr;
};

class lock_free_thread_pool
{
	std::atomic_flag done = ATOMIC_FLAG_INIT;
	vector<lock_free_task> work_queue;
	std::vector<std::jthread> threads;

	void worker_thread(int threadIndex)
	{
		while (!done.test(memory_order::relaxed))
		{
			if (!work_queue[threadIndex].isDone.test(memory_order::relaxed))
			{
				work_queue[threadIndex].task();
				work_queue[threadIndex].task = nullptr;
				work_queue[threadIndex].isDone.test_and_set(memory_order::relaxed);
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
public:
	lock_free_thread_pool()
		: work_queue{ vector<lock_free_task>(std::thread::hardware_concurrency() - 1)}
	{
		done.clear(memory_order::relaxed);
		unsigned const thread_count = std::thread::hardware_concurrency() - 1;
		try
		{
			for (unsigned i = 0; i < thread_count; ++i)
			{
				work_queue[i].isDone.test_and_set(memory_order::relaxed);
				threads.push_back(std::jthread(&lock_free_thread_pool::worker_thread, this, i));
			}
		}
		catch (...)
		{
			done.test_and_set(memory_order::relaxed);
			throw;
		}
	}
	~lock_free_thread_pool()
	{
		done.test_and_set(memory_order::relaxed);
	}

	template<typename FunctionType>
	void submit(FunctionType f)
	{
		auto cnt = 0;

		for (cnt; cnt < work_queue.size(); ++cnt)
		{
			if (work_queue[cnt].isDone.test(memory_order::relaxed))
			{
				break;
			}
		}

		if (cnt >= work_queue.size())
		{
			f();
		}
		else
		{
			work_queue[cnt].task = f;
			work_queue[cnt].isDone.clear(memory_order::relaxed);
		}
	}

	void waitForAllTaskToFinish()
	{
		while (true)
		{
			auto finCnt = 0;

			for (auto cnt = 0; cnt < work_queue.size(); ++cnt)
			{
				if (work_queue[cnt].isDone.test(memory_order::relaxed))
				{
					++finCnt;
				}
				else
				{
					break;
				}
			}

			if (finCnt >= work_queue.size())
			{
				break;
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
};

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

//Exact version of: https://stackoverflow.com/questions/28509273/get-types-of-c-function-parameters
template<typename Sig>
struct signature;
template<typename Ret, typename...Args>
struct signature<Ret(Args...)> {
	using type = tuple<Args...>;
};

template<typename Ret, typename Obj, typename...Args>
struct signature<Ret(Obj::*)(Args...)> {
	using type = tuple<Args...>;
};
template<typename Ret, typename Obj, typename...Args>
struct signature<Ret(Obj::*)(Args...)const> {
	using type = tuple<Args...>;
};
template<typename Fun>
concept is_fun = is_function_v<Fun>;

template<typename Fun>
concept is_mem_fun = is_member_function_pointer_v<decay_t<Fun>>;

template<typename Fun>
concept is_functor = is_class_v<decay_t<Fun>> && requires(Fun && t) {
	&decay_t<Fun>::operator();
};

template<is_functor T>
auto arguments(T&& t) -> signature<decltype(&decay_t<T>::operator())>::type;

template<is_functor T>
auto arguments(const T& t) -> signature<decltype(&decay_t<T>::operator())>::type;

// template<is_fun T>
// auto arguments(T&& t)->signature<T>::type;

template<is_fun T>
auto arguments(const T& t) -> signature<T>::type;

template<is_mem_fun T>
auto arguments(T&& t) -> signature<decay_t<T>>::type;

template<is_mem_fun T>
auto arguments(const T& t) -> signature<decay_t<T>>::type;
struct functor {
	int operator()(const string&, double) {
		return 0;
	}
};

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

template <typename T>
class move_only_invoke_and_destroy_func_32;

// Modified version of: https://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented
// Uses memcopy and memset so don't use for dynamically allocated members (can cause memory leaks)
// Warning / Rule: Don't use it to store function objects that have dynamically allocated members
template <typename... Args>
class move_only_invoke_and_destroy_func_32<void(Args...)>
{
	constexpr static auto BUFFER_SIZE = 32;
	constexpr static auto BUFFER_ALIGNMENT = 8;

	// function pointer types for the type-erasure behaviors
	// all these std::byte* parameters are actually casted from some functor type
	typedef void(*invoke_fn_t)(std::byte*, Args&&...);
	typedef void (*destroy_fn_t)(std::byte*);

	// type-aware generic functions for invoking
	// the specialization of these functions won't be capable with
	// the above function pointer types, so we need some cast
	template <typename Functor>
	static void invoke_fn(Functor* fn, Args&&... args)
	{
		(*fn)(std::forward<Args>(args)...);
	}

	template <typename Functor>
	static void destroy_fn(Functor* f)
	{
		f->~Functor();
	}

	// these pointers are storing behaviors
	invoke_fn_t invoke_f;
	destroy_fn_t destroy_f;

	// erase the type of any functor and store it into a std::byte*
	// so the storage size should be obtained as well
	mutable alignas(BUFFER_ALIGNMENT) std::byte data_ptr[BUFFER_SIZE];
public:
	move_only_invoke_and_destroy_func_32()
		: invoke_f(nullptr)
		, destroy_f(nullptr)
	{
		std::memset(this->data_ptr, 0x0, BUFFER_SIZE);
	}

	// construct from any functor type
	template <typename Functor>
	move_only_invoke_and_destroy_func_32(Functor&& f) noexcept
		// specialize functions and erase their type info by casting
		: invoke_f(reinterpret_cast<invoke_fn_t>(invoke_fn<Functor>))
		, destroy_f(reinterpret_cast<destroy_fn_t>(destroy_fn<Functor>))
	{
		// copy the functor to internal storage
		new (this->data_ptr) Functor(std::move(f));
	}

	move_only_invoke_and_destroy_func_32(move_only_invoke_and_destroy_func_32 const& rhs) noexcept = delete;
	move_only_invoke_and_destroy_func_32& operator= (move_only_invoke_and_destroy_func_32 const& rhs) = delete;

	move_only_invoke_and_destroy_func_32(move_only_invoke_and_destroy_func_32&& rhs) noexcept
		: invoke_f(std::exchange(rhs.invoke_f, nullptr))
		, destroy_f(std::exchange(rhs.destroy_f, nullptr))
	{
		if (this->invoke_f) {
			std::memcpy(this->data_ptr, rhs.data_ptr, BUFFER_SIZE);
			std::memset(rhs.data_ptr, 0x00, BUFFER_SIZE);
		}
	}

	move_only_invoke_and_destroy_func_32& operator=(move_only_invoke_and_destroy_func_32&& rhs) noexcept
	{
		invoke_f = std::exchange(rhs.invoke_f, nullptr);
		destroy_f = std::exchange(rhs.destroy_f, nullptr);

		if (this->invoke_f) {
			std::memcpy(this->data_ptr, rhs.data_ptr, BUFFER_SIZE);
			std::memset(rhs.data_ptr, 0x00, BUFFER_SIZE);
		}
		return *this;
	};

	~move_only_invoke_and_destroy_func_32() noexcept
	{
		if (destroy_f != nullptr) {
			this->destroy_f(this->data_ptr);
			invoke_f = nullptr;
			destroy_f = nullptr;
			std::memset(this->data_ptr, 0x00, BUFFER_SIZE);
		}
	}

	void operator()(Args&&... args)
	{
		this->invoke_f(this->data_ptr, std::forward<Args>(args)...);
		//ToDo: Need more test whether this improves performance specially when a class allocates in heap while in heap.
		//this->~move_only_invoke_and_destroy_func_32();
	}
};

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//Modified version of: https://www.delftstack.com/howto/cpp/cpp-vector-implementation/

class clustering_task_queue
{
public:
	inline clustering_task_queue() noexcept
		: cap{ 0 }
		, elems{ 0 }
		, arr{ nullptr }
	{
	};

	inline clustering_task_queue(size_t n) noexcept
		: cap{ n }
		, elems{ 0 }
		, arr{ new move_only_invoke_and_destroy_func_32<void()>[n] }

	{
	};

	clustering_task_queue(clustering_task_queue const& rhs) noexcept = delete;
	clustering_task_queue& operator= (clustering_task_queue const& rhs) = delete;

	inline clustering_task_queue(clustering_task_queue&& rhs) noexcept
		: cap(std::exchange(rhs.cap, 0))
		, elems(std::exchange(rhs.elems, 0))
		, arr(std::exchange(rhs.arr, nullptr))
	{
	}

	inline clustering_task_queue& operator=(clustering_task_queue&& rhs) noexcept
	{
		cap = std::exchange(rhs.cap, 0);
		elems = std::exchange(rhs.elems, 0);
		arr = std::exchange(rhs.arr, nullptr);

		return *this;
	};

	inline void resize_and_emplace(move_only_invoke_and_destroy_func_32<void()>&& data) noexcept
	{
		if (elems < cap)
		{
			new (this->arr + elems) move_only_invoke_and_destroy_func_32<void()>(std::move(data));
			++elems;
		}
		else
		{
			auto tmp_arr = new move_only_invoke_and_destroy_func_32<void()>[cap * 2];
			cap *= 2;
			for (auto i = 0; i < elems; i++)
			{
				tmp_arr[i] = std::move(arr[i]);
			}
			delete[] arr;
			arr = tmp_arr;

			new (this->arr + elems) move_only_invoke_and_destroy_func_32<void()>(std::move(data));
			++elems;
		}
	}
	inline void clear(bool isCleanUp = true) noexcept
	{
		elems = 0;

		if (isCleanUp)
		{
			for (auto i = 0; i < elems; i++)
			{
				arr[i].~move_only_invoke_and_destroy_func_32<void()>();
			}
		}
	}

	[[nodiscard]] inline bool empty() const noexcept
	{
		return elems == 0;
	}
	[[nodiscard]] inline size_t size() const noexcept
	{
		return elems;
	}
	[[nodiscard]] inline size_t capacity() const noexcept
	{
		return cap;
	}
	move_only_invoke_and_destroy_func_32<void()>& operator[](size_t pos) 
	{
		if (pos >= 0 && pos <= elems)
		{
			return *(this->arr + pos);
		}
		throw std::out_of_range("Out of bounds element access");
	}

	inline ~clustering_task_queue() noexcept 
	{
		delete[] arr;
	}
private:
	size_t cap;
	size_t elems;
	move_only_invoke_and_destroy_func_32<void()>* arr = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct clustering_ptr;

template<class T>
struct funcWrapperToCluster final
{
	unsigned int bufferId = 0;
	
	inline std::byte* get(clustering_ptr<T>& ptr) noexcept
	{
		return ptr.get()->buffer[bufferId];
	}
	inline std::byte const* const get(const clustering_ptr<T>& ptr) const noexcept
	{
		return ptr.get()->buffer[bufferId];
	}

	inline T* getThis(clustering_ptr<T>& ptr) noexcept { return ptr.get(); }
	inline T const* const getThis(const clustering_ptr<T>& ptr) const noexcept { return ptr.get(); }
};

template<class T, typename memfn, typename Args>
struct funcWrapper final
{
	mutable clustering_ptr<T> clusterPtr;
	mutable memfn func;
	mutable funcWrapperToCluster<T> inter;

	inline funcWrapper(clustering_ptr<T>& ptr, memfn& fn, unsigned int& bufferId) noexcept
		: clusterPtr{ ptr }
		, func{ fn }
		, inter{ bufferId }
	{
	}

	inline funcWrapper(const funcWrapper& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
		, inter {std::exchange(other.inter, funcWrapperToCluster<T>())}
	{
	}

	inline funcWrapper& operator=(const funcWrapper& other) noexcept
	{
		clusterPtr = other.clusterPtr;
		func = std::exchange(other.func, nullptr);
		inter = std::exchange(other.inter, funcWrapperToCluster<T>());
		return *this;
	}

	funcWrapper(funcWrapper&& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
		, inter{ std::exchange(other.inter, funcWrapperToCluster<T>()) }
	{
	}
	funcWrapper& operator=(funcWrapper&& other)
	{
		clusterPtr = other.clusterPtr;
		func = std::exchange(other.func, nullptr);
		inter = std::exchange(other.inter, funcWrapperToCluster<T>());
		return *this;
	}

	inline ~funcWrapper() noexcept
	{
		if (func != nullptr)
		{
			auto ptr = Get();
			ptr->~Args();
			std::memset(ptr, 0, sizeof(ptr));
		}
	}

	inline Args* Get() noexcept
	{
		return reinterpret_cast<Args*>(inter.get(clusterPtr));
	}

	inline const Args* Get() const noexcept
	{
		return &reinterpret_cast<const Args*>(inter.get(clusterPtr));
	}

	inline void operator()() noexcept
	{
		auto args = std::make_tuple(inter.getThis(clusterPtr));
		auto tupArgs = std::tuple_cat(args, std::move(*Get()));

		//works for void(T) and void (T&&)
		if constexpr (std::is_rvalue_reference<decltype(std::get<0>(arguments(func)))>::value)
		{
			std::apply(std::move(func), std::move(tupArgs));
		}
		//works for void(T&)
		else
		{
			std::apply(func, tupArgs);
		}
	}
};

template<class T>
struct funcWrapperToClusterVoid final
{
	inline T* getThis(clustering_ptr<T>& ptr) noexcept { return ptr.get(); }
	inline T const* const getThis(const clustering_ptr<T>& ptr) const noexcept { return ptr.get(); }
};

template<class T, typename memfn>
struct funcWrapperVoid final
{
	mutable clustering_ptr<T> clusterPtr;
	mutable memfn func;
	mutable funcWrapperToClusterVoid<T> inter;

	inline funcWrapperVoid(clustering_ptr<T>& ptr, memfn& fn) noexcept
		: clusterPtr{ ptr }
		, func{ fn }
	{
	}

	inline funcWrapperVoid(const funcWrapperVoid& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
	{
	}

	inline funcWrapperVoid& operator=(const funcWrapperVoid& other) noexcept
	{
		clusterPtr = other.clusterPtr;
		func = std::exchange(other.func, nullptr);
		return *this;
	}

	funcWrapperVoid(funcWrapperVoid&& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
	{
	}
	funcWrapperVoid& operator=(funcWrapperVoid&& other)
	{
		clusterPtr = other.clusterPtr;
		func = std::exchange(other.func, nullptr);
		return *this;
	}

	inline ~funcWrapperVoid() noexcept = default;

	inline void operator()() noexcept
	{
		std::invoke(func, inter.getThis(clusterPtr));
	}
};

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock = std::vector<T>();
	clustering_task_queue taskQueue = clustering_task_queue();
	std::unique_ptr<atomic_flag> isBeingAccessed = std::make_unique<atomic_flag>();
};

template<class T>
struct rw_clustering_ptr;
template<class T>
struct ClusteringMemoryPool;

template<class T>
struct clustering_ptr
{
private:
	inline T* operator->()		{ return &((*poolHeadPtr).m_memory_pool[clusterId].dataBlock[index]); }
	inline T* get()			
	{ 
		return &((*poolHeadPtr).m_memory_pool[clusterId].dataBlock[index]);
	}

public:
	friend struct rw_clustering_ptr<T>;
	friend struct funcWrapperToCluster<T>;
	friend struct funcWrapperToClusterVoid<T>;

	inline clustering_ptr() = default;

	inline clustering_ptr(ClusteringMemoryPool<T>* _poolHeadPtr, unsigned int _clusterId, unsigned int _index)
		: poolHeadPtr { _poolHeadPtr }
		, clusterId { _clusterId }
		, index {_index}
	{
	}

	inline clustering_ptr(const clustering_ptr& rhs) noexcept
		: poolHeadPtr { rhs.poolHeadPtr }
		, clusterId { rhs.clusterId }
		, index { rhs.index }
	{
	}

	inline clustering_ptr& operator=(const clustering_ptr& rhs) noexcept
	{
		poolHeadPtr = rhs.poolHeadPtr;
		clusterId = rhs.clusterId;
		index = rhs.index;
		return *this;
	}

	inline clustering_ptr(clustering_ptr&& rhs) noexcept
		: poolHeadPtr{ std::exchange(rhs.poolHeadPtr, nullptr) }
		, clusterId{ std::exchange(rhs.clusterId, 0) }
		, index{ std::exchange(rhs.index, 0) }
	{
	}

	inline clustering_ptr& operator=(clustering_ptr&& rhs) noexcept
	{
		poolHeadPtr = std::exchange(rhs.poolHeadPtr, nullptr);
		clusterId = std::exchange(rhs.clusterId, 0);
		index = std::exchange(rhs.index, 0);
		return *this;
	}

	inline ~clustering_ptr() noexcept
	{
		poolHeadPtr = nullptr;
		clusterId = 0;
		index = 0;
	}

	ClusteringMemoryPool<T>* poolHeadPtr = nullptr;		//size 8
	unsigned int clusterId = 0;										//size 4
	unsigned int index = 0;											//size 4

	T const* const operator-> () const	{ return &((*poolHeadPtr).m_memory_pool[clusterId].dataBlock[index]); }
	T const* const get() const			{ return &((*poolHeadPtr).m_memory_pool[clusterId].dataBlock[index]); }
};

template<class T>
struct rw_clustering_ptr final
{
private:
	clustering_ptr<T> ptr;

public:
	explicit rw_clustering_ptr() = default;
	explicit rw_clustering_ptr(ClusteringMemoryPool<T>* poolHeadPtr, unsigned int clusterId, unsigned int index) : ptr{ poolHeadPtr, clusterId, index} {}

	inline rw_clustering_ptr(const rw_clustering_ptr& rhs) noexcept
		: ptr{ rhs.ptr }
	{
	}

	inline rw_clustering_ptr& operator=(const rw_clustering_ptr& rhs) noexcept
	{
		ptr = rhs.ptr;
		return *this;
	}

	inline rw_clustering_ptr(rw_clustering_ptr&& rhs) noexcept 
		: ptr{ std::move(rhs.ptr) }
	{
	}
	inline rw_clustering_ptr& operator=(rw_clustering_ptr&& rhs) noexcept 
	{
		ptr = std::move(rhs.ptr);
		return *this;
	}

	T const* const operator-> () const	{ return ptr.get(); }
	T const* const get() const			{ return ptr.get(); }
	bool isValid() const				{ return ptr.poolHeadPtr != nullptr; }
	int getClusterId() const			{ return ptr.clusterId; }
	int getIndex() const				{ return ptr.index; }

	//Use for tasks that are simple
	//Immediate write like a normal function call
	//Sig: Accepts void(T&&...)
	template<typename memfn, typename... Args>
	void invoke(memfn&& func, Args&&... args)
	{
		std::invoke(std::forward<memfn>(func), ptr.get(), std::forward<Args>(args)...);
	}

	//Don't use for tasks that are simple
	//Write task only pushed to the queue once and invalidated subsequent push to the queue until the queue has been executed
	//Sig: Accepts void(T&&...) with all parameters of the same reference type
	template<typename memfn, typename... Args>
	void oneTimeWrite(unsigned int bufferId, memfn&& func, Args&&... args)
	{
		static_assert(!std::is_base_of<POD, T>::value, "Plain old data setters should not be complex, use invoke or stackingWrite instead and don't use ClusterableWithBuffer!!");

		auto staticCheckForCharArray= [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_same_v<std::decay_t<packArg>, char*> && !std::is_same_v<std::decay_t<packArg>, const char*>, "Has char array!! Use SimpleString instead!"); };
		(staticCheckForCharArray(std::forward<Args>(args)), ...);

		auto staticCheckForString = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_same_v<std::decay_t<packArg>, string>, "Has std::string!! Use SimpleString instead!"); };
		(staticCheckForString(std::forward<Args>(args)), ...);

		while ((*ptr.poolHeadPtr).m_memory_pool[ptr.clusterId].isBeingAccessed->test_and_set(memory_order::acquire))
		{
			std::this_thread::yield();
		}
		
		//ToDo: check whether all args are of the same reference type(i.e all lvalue or all rvalue)
		//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
		//(staticCheckForLvalue(std::forward<Args>(args)), ...);

		byte* buf = ptr.get()->buffer[bufferId];
		byte testblock[sizeof(buf) / sizeof(*buf)];
		memset(testblock, 0, sizeof(testblock));

		if (!memcmp(testblock, buf, (sizeof(buf) / sizeof(*buf))))
		{
			auto& queue = (*ptr.poolHeadPtr).m_memory_pool[ptr.clusterId].taskQueue;
			using t = tuple<decay_t<Args>...>;
			new (buf) t(std::make_tuple(std::move(args)...));
			move_only_invoke_and_destroy_func_32<void()> defferedWrite = funcWrapper<T, memfn, t>(ptr, func, bufferId);
			queue.resize_and_emplace(std::move(defferedWrite));
			//auto size = queue.size();		
			//queue.resize(size + 1);
			//new (&queue[size]) move_only_invoke_and_destroy_func_32<void()>();
		}

		(*ptr.poolHeadPtr).m_memory_pool[ptr.clusterId].isBeingAccessed->clear(memory_order::release);
	}

	//Prefer this over oneTimeWrite if you can for deffered writes
	//Fast push to the write task queue. Write to the same object can be stacked and will be logically cohorent
	//Sig: Accepts void()
	template<typename memfn>
	void stackingWrite(memfn&& func)
	{
		while ((*ptr.poolHeadPtr).m_memory_pool[ptr.clusterId].isBeingAccessed->test_and_set(memory_order::acquire))
		{
			std::this_thread::yield();
		}

		auto& queue = (*ptr.poolHeadPtr).m_memory_pool[ptr.clusterId].taskQueue;
		move_only_invoke_and_destroy_func_32<void()> defferedWrite = funcWrapperVoid<T, memfn>(ptr, func);
		queue.resize_and_emplace(std::move(defferedWrite));
		//auto size = queue.size();
		//queue.resize(size + 1);
		//new (&queue[size]) move_only_invoke_and_destroy_func_32<void()>(std::move(defferedWrite));

		(*ptr.poolHeadPtr).m_memory_pool[ptr.clusterId].isBeingAccessed->clear(memory_order::release);
	}
};

struct MemoryPool {};

template<class T>
class ClusteringMemoryPool : public MemoryPool
{
public:
	explicit ClusteringMemoryPool() = delete;
	explicit ClusteringMemoryPool(unsigned int block_size) : m_memory_pool{ std::vector<DataTaskBlockPair<T>>() }, m_block_size{ block_size }
	{
	}

	rw_clustering_ptr<T> AddToPool(T&& obj)
	{
		if(m_memory_pool.size() <= 0)
		{
			auto vec = std::vector<T>();
			vec.reserve(m_block_size);
			auto vec2 = clustering_task_queue(m_block_size);
			m_memory_pool.emplace_back(DataTaskBlockPair<T>(std::move(vec), std::move(vec2)));
			return AddToPool(std::move(obj));
		}
		else
		{
			auto& lastPool = m_memory_pool[m_memory_pool.size() - 1];
			if (lastPool.dataBlock.size() < m_block_size)
			{
				auto ptr = rw_clustering_ptr{ this, static_cast<unsigned int>(m_memory_pool.size() - 1), static_cast<unsigned int>(lastPool.dataBlock.size()) };
				lastPool.dataBlock.emplace_back(std::move(obj));
				return ptr;
			}
			else
			{
				auto vec = std::vector<T>();
				vec.reserve(m_block_size);
				auto vec2 = clustering_task_queue(m_block_size);
				m_memory_pool.emplace_back(DataTaskBlockPair<T>(std::move(vec), std::move(vec2)));
				return AddToPool(std::move(obj));
			}
		}
	}
	
	void ExecuteClusters(int startId, int endId)
	{
		for (int poolId = startId; poolId < endId; ++poolId)
		{
			if (poolId >= m_memory_pool.size()) { break; }

			for (auto taskId = 0; taskId < m_memory_pool[poolId].taskQueue.size(); ++taskId)
			{
				m_memory_pool[poolId].taskQueue[taskId]();
			}
			m_memory_pool[poolId].taskQueue.clear();
		}
	}

	void ExecuteClusteredTasksParallel(lock_free_thread_pool& pool, bool isWaitTillFinish)
	{
		unsigned int poolSize = m_memory_pool.size();
		auto threadCount = std::min(poolSize, std::thread::hardware_concurrency());

		if (threadCount == 1)
		{
			for (auto& pool : m_memory_pool)
			{
				for (auto taskId = 0; taskId < pool.taskQueue.size(); ++taskId)
				{
					pool.taskQueue[taskId]();
				}
				pool.taskQueue.clear();
			}
		}
		else
		{
			for (int threadId = 0; threadId < threadCount; ++threadId)
			{
				auto groupCount = static_cast<unsigned int>(ceilf((float)poolSize / threadCount));
				auto startId = threadId * groupCount;
				auto endId = (threadId + 1) * groupCount;
				auto task = [startId = startId, endId = endId, this]() {std::invoke(&ClusteringMemoryPool<T>::ExecuteClusters, this, startId, endId); };
				pool.submit(task);
			}

			if(isWaitTillFinish)
			{
				pool.waitForAllTaskToFinish();
			}
		}
	}

	~ClusteringMemoryPool() = default;

	std::vector<DataTaskBlockPair<T>> m_memory_pool;
private:
	unsigned int m_block_size;
};

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html#:~:text=Entity%2DComponent%2DSystem%20(ECS,ECS%20frameworks%20are%20not%20trivial
//https://austinmorlan.com/posts/entity_component_system/

std::atomic<int> s_poolCounter;
template <class T>
int GetPoolId()
{
	static int s_poolId = s_poolCounter++;
	return s_poolId;
}

class Scene
{
private:
	struct Component
	{
		int poolId;
		int clusterId;
		int componentId;
	};

	struct Entity;
	ClusteringMemoryPool<Entity> entities;
	std::vector<MemoryPool*> memoryPools;
	std::vector<rw_clustering_ptr<Entity>> freeEntities;

	template<typename T>
	rw_clustering_ptr<T> ComponentToPtr(Component& component)
	{
		int poolId = GetPoolId<T>();
		auto poolHead = (static_cast<ClusteringMemoryPool<T>*>(memoryPools[poolId]));		//warning: may cause undefined behaviour
		return rw_clustering_ptr<T>(poolHead, component.clusterId, component.componentId);
	}

	template<typename T>
	rw_clustering_ptr<T> ComponentToPtr(rw_clustering_ptr<Entity> entity, int index)
	{
		auto component = entity->GetComponent(index);
		return ComponentToPtr<T>(component);
	}

	template<typename T>
	Component PtrToComponent(rw_clustering_ptr<T>& ptr_data)
	{
		int poolId = GetPoolId<T>();
		return Component{ poolId, ptr_data.getClusterId(), ptr_data.getIndex() };
	}

public:
	struct Entity : ClusterableWithBuffer<sizeof(Component), alignof(Component)>
	{
		friend class Scene;
		std::vector<Component> const * GetAllComponents() const
		{
			return &components;
		}

		Component GetComponent(int index) const
		{
			return components[index];
		}

	private:
		void AddComponent(Component&& component)
		{
			components.emplace_back(std::move(component));
		}

		std::vector<Component> components;
	};

	Scene(unsigned int entityBlockSize) 
		: entities{ ClusteringMemoryPool<Entity>(entityBlockSize) }
		, memoryPools{ std::vector<MemoryPool*>() }
		, freeEntities{ std::vector<rw_clustering_ptr<Entity>>() }
	{}
	~Scene() = default;

	rw_clustering_ptr<Entity> NewEntity()
	{
		if (!freeEntities.empty())
		{
			auto newIndex = freeEntities.back();
			freeEntities.pop_back();
			return newIndex;
		}
		return entities.AddToPool(Entity());
	}

	template<typename T>
	rw_clustering_ptr<T> AssignComponent(rw_clustering_ptr<Entity>& entity, T&& data, unsigned int poolSize = 1000)
	{
		int poolId = GetPoolId<T>();

		if (memoryPools.size() <= poolId) // Not enough component pool
		{
			memoryPools.resize(poolId + 1, nullptr);
		}
		if (memoryPools[poolId] == nullptr) // New component, make a new pool
		{
			memoryPools[poolId] = new ClusteringMemoryPool<T>(poolSize);
		}

		// Looks up the component in the pool, and initializes it with placement new
		rw_clustering_ptr<T> pData = (static_cast<ClusteringMemoryPool<T>*>(memoryPools[poolId]))->AddToPool(std::move(data)); //warning: may cause undefined behaviour
		entity.oneTimeWrite(0, &Entity::AddComponent, PtrToComponent<T>(pData));
		return pData;
	}

	template<typename T>
	void ExecuteClusteredTasksParallel(lock_free_thread_pool& pool, bool isWaitrForFinish)
	{
		if constexpr (std::is_same_v<T, Entity>)
		{
			entities.ExecuteClusteredTasksParallel(pool, isWaitrForFinish);
		}
		else
		{
			int poolId = GetPoolId<T>();
			(static_cast<ClusteringMemoryPool<T>*>(memoryPools[poolId]))->ExecuteClusteredTasksParallel(pool, isWaitrForFinish);
		}
	}

	//ToDo: Finish later
	//template<typename T>
	//void Remove(rw_clustering_ptr<T> ptr)
	//{
	//	// ensures you're not accessing an entity that has been deleted
	//	if (entities[GetEntityIndex(id)].id != id)
	//		return;

	//	int componentId = GetId<T>();
	//	entities[GetEntityIndex(id)].mask.reset(componentId);
	//}

	//void DestroyEntity(EntityID id)
	//{
	//	EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(id) + 1);
	//	entities[GetEntityIndex(id)].id = newID;
	//	entities[GetEntityIndex(id)].mask.reset();
	//	freeEntities.push_back(GetEntityIndex(id));
	//}

	//template<typename T>
	//rw_clustering_ptr<T> Get(EntityID id)
	//{
	//	int componentId = GetId<T>();
	//	if (!entities[id].mask.test(componentId))
	//		return nullptr;

	//	T* pComponent = static_cast<T*>(componentPools[componentId]->get(id));
	//	return pComponent;
	//}
};

struct Stage 
{
	//memory pool for the systems
	std::vector<System*> systemPool;	
};

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

#endif
