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

using namespace std;

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

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

template<typename T, typename... Args>
using mem_func_ptr = void (T::*)(Args&&... args);

template <unsigned int size = 1, unsigned int alignment = 1, unsigned int count = 1, class... Args>
struct ClusterableWithBuffer
{
	alignas(alignment) std::byte buffer[count][size];
	tuple<Args...> funcTups;

	ClusterableWithBuffer() noexcept
	{
		std::memset(buffer, 0, size);
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

template<class T>
struct funcWrapper;

template<class T>
class clustering_task_queue
{
public:
	inline clustering_task_queue(size_t n) noexcept
		: cap{ n }
		, elems{ 0 }
		, arr{ new funcWrapper<T>[n] }

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

	inline void resize() noexcept
	{
		if (elems < cap)
		{
			++elems;
		}
		else
		{
			auto tmp_arr = new funcWrapper<T>[cap * 2];
			cap *= 2;
			for (auto i = 0; i < elems; i++)
			{
				tmp_arr[i] = std::move(arr[i]);
			}
			delete[] arr;
			arr = tmp_arr;

			++elems;
		}
	}

	inline void resize_and_emplace(funcWrapper<T>&& data) noexcept
	{
		if (elems < cap)
		{
			new (this->arr + elems) funcWrapper<T>(std::move(data));
			++elems;
		}
		else
		{
			auto tmp_arr = new funcWrapper<T>[cap * 2];
			cap *= 2;
			for (auto i = 0; i < elems; i++)
			{
				tmp_arr[i] = std::move(arr[i]);
			}
			delete[] arr;
			arr = tmp_arr;

			new (this->arr + elems) funcWrapper<T>(std::move(data));
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
				arr[i].~funcWrapper<T>();
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
	funcWrapper<T>& operator[](size_t pos)
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
	funcWrapper<T>* arr = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct clustering_ptr;

template<class T>
struct funcWrapperToCluster final
{
	short bufferId = -1;
	
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

template <typename ... Ts>
constexpr auto decay_types(std::tuple<Ts...> const&)
-> std::tuple<std::remove_cv_t<std::remove_reference_t<Ts>>...>;

template <typename T>
using decay_tuple = decltype(decay_types(std::declval<T>()));

template<class T>
struct funcWrapper final
{
	mutable clustering_ptr<T> clusterPtr;
	mutable funcWrapperToCluster<T> inter;

	inline funcWrapper() = default;

	inline funcWrapper(clustering_ptr<T>& ptr) noexcept
		: clusterPtr{ ptr }
		, inter{ funcWrapperToCluster<T>() }
	{
	}

	inline funcWrapper(clustering_ptr<T>& ptr, short& bufferId) noexcept
		: clusterPtr{ ptr }
		, inter{ bufferId }
	{
	}

	inline funcWrapper(const funcWrapper& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, inter {std::exchange(other.inter, funcWrapperToCluster<T>())}
	{
	}

	inline funcWrapper& operator=(const funcWrapper& other) noexcept
	{
		clusterPtr = other.clusterPtr;
		inter = std::exchange(other.inter, funcWrapperToCluster<T>());
		return *this;
	}

	funcWrapper(funcWrapper&& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, inter{ std::exchange(other.inter, funcWrapperToCluster<T>()) }
	{
	}
	funcWrapper& operator=(funcWrapper&& other)
	{
		clusterPtr = other.clusterPtr;
		inter = std::exchange(other.inter, funcWrapperToCluster<T>());
		return *this;
	}

	inline ~funcWrapper() noexcept
	{
		if(clusterPtr.poolHeadPtr != nullptr && inter.bufferId != -1)
		{
			auto ptr = Get();
			auto func = std::get<0>(inter.getThis(clusterPtr)->funcTups);
			using Args = decay_tuple<decltype(arguments(func))>;
			ptr->~Args();
			std::memset(ptr, 0, sizeof(ptr));
		}
		clusterPtr.poolHeadPtr = nullptr;
		inter.bufferId = -1;
	}

	inline auto Get() noexcept
	{
		auto func = std::get<0>(inter.getThis(clusterPtr)->funcTups);
		using Args = decay_tuple<decltype(arguments(func))>;
		return reinterpret_cast<Args*>(inter.get(clusterPtr));
	}

	inline const auto Get() const noexcept
	{
		auto func = std::get<0>(inter.getThis(clusterPtr)->funcTups);
		using Args = decay_tuple<decltype(arguments(func))>;
		return &reinterpret_cast<const Args*>(inter.get(clusterPtr));
	}

	inline void operator()() noexcept
	{
		auto func = std::get<0>(inter.getThis(clusterPtr)->funcTups);

		if constexpr (std::is_same_v<decltype(func), void(T::*)()>)
		{
			std::invoke(func, inter.getThis(clusterPtr));
		}
		else
		{
			auto args = std::make_tuple(inter.getThis(clusterPtr));
			auto tupArgs = std::tuple_cat(args, std::move(*Get()));

			//works for void(T) and void (T&&)
			if constexpr (std::is_rvalue_reference_v<decltype(std::get<0>(arguments(func)))>)
			{
				std::apply(std::move(func), std::move(tupArgs));
			}
			//works for void(T&)
			else
			{
				std::apply(func, tupArgs);
			}
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	clustering_task_queue<T> taskQueue;
	std::byte padding[16];
};

template<class T>
struct rw_clustering_ptr;
template<class T>
struct ClusteringMemoryPool;

template<class T>
struct clustering_ptr
{
private:
	inline T* operator->()		{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
	inline T* get()			
	{ 
		return &((*poolHeadPtr)[clusterId].dataBlock[index]); 
	}

public:
	friend struct rw_clustering_ptr<T>;
	friend struct funcWrapperToCluster<T>;

	inline clustering_ptr() = default;

	inline clustering_ptr(std::vector<DataTaskBlockPair<T>>* _poolHeadPtr, unsigned int _clusterId, unsigned int _index)
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

	clustering_ptr(clustering_ptr&& rhs) noexcept = delete;
	clustering_ptr& operator=(clustering_ptr&& rhs) noexcept = delete;

	inline ~clustering_ptr() noexcept
	{
		poolHeadPtr = nullptr;
		clusterId = 0;
		index = 0;
	}

	std::vector<DataTaskBlockPair<T>>* poolHeadPtr = nullptr;		//size 8
	unsigned int clusterId = 0;										//size 4
	unsigned int index = 0;											//size 4

	T const* const operator-> () const	{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
	T const* const get() const			{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
};

template<class T>
struct rw_clustering_ptr final
{
private:
	clustering_ptr<T> ptr;

public:
	explicit rw_clustering_ptr() = default;
	explicit rw_clustering_ptr(std::vector<DataTaskBlockPair<T>>* poolHeadPtr, unsigned int clusterId, unsigned int index) : ptr{ poolHeadPtr, clusterId, index} {}

	T const* const operator-> () const	{ return ptr.get(); }
	T const* const get() const			{ return ptr.get(); }
	bool isValid() const				{ return ptr.poolHeadPtr != nullptr; }


	//Immediate write like a normal function call
	//Sig: Accepts void(T&&...)
	template<typename memfn, typename... Args>
	void invoke(memfn&& func, Args&&... args)
	{
		std::invoke(std::forward<memfn>(func), ptr.get(), std::forward<Args>(args)...);
	}

	//Write task only pushed to the queue once and invalidated subsequent push to the queue until the queue has been executed
	//Sig: Accepts void(T&&...) with all parameters of the same reference type
	template<typename memfn, typename... Args>
	void oneTimeWrite(short bufferId, memfn&& func, Args&&... args)
	{
		auto staticCheckForCharArray= [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_same_v<std::decay_t<packArg>, char*> && !std::is_same_v<std::decay_t<packArg>, const char*>, "Has char array!! Use SimpleString instead!"); };
		(staticCheckForCharArray(std::forward<Args>(args)), ...);

		auto staticCheckForString = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_same_v<std::decay_t<packArg>, string>, "Has std::string!! Use SimpleString instead!"); };
		(staticCheckForString(std::forward<Args>(args)), ...);

		//ToDo: check whether all args are of the same reference type(i.e all lvalue or all rvalue)
		//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
		//(staticCheckForLvalue(std::forward<Args>(args)), ...);

		byte* buf = ptr.get()->buffer[bufferId];
		byte testblock[sizeof(buf) / sizeof(*buf)];
		memset(testblock, 0, sizeof(testblock));

		if (!memcmp(testblock, buf, (sizeof(buf) / sizeof(*buf))))
		{
			auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
			using t = tuple<decay_t<Args>...>;
			new (buf) t(std::make_tuple(std::move(args)...));
			queue.resize_and_emplace(std::move(funcWrapper<T>(ptr, bufferId)));
			//auto size = queue.size();		
			//queue.resize();
			//new (&queue[size]) funcWrapper<T>(ptr, bufferId);
		}
	}


	//Fast push to the write task queue. Write to the same object can be stacked and will be logically cohorent
	//Sig: Accepts void()
	template<typename memfn>
	void stackingWrite(memfn&& func)
	{
		auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
		queue.resize_and_emplace(std::move(funcWrapper<T>(ptr)));
		//auto size = queue.size();
		//queue.resize();
		//new (&queue[size]) funcWrapper<T>(ptr);
	}
};

template<class T>
class ClusteringMemoryPool
{
public:
	explicit ClusteringMemoryPool() = delete;
	explicit ClusteringMemoryPool(unsigned int block_size) : m_memory_pool {std::vector<DataTaskBlockPair<T>>()}, m_block_size{block_size}
	{
	}

	rw_clustering_ptr<T> AddToPool(T&& obj)
	{
		if(m_memory_pool.size() <= 0)
		{
			auto vec = std::vector<T>();
			vec.reserve(m_block_size);
			auto vec2 = clustering_task_queue<T>(m_block_size);
			m_memory_pool.emplace_back(DataTaskBlockPair<T>{std::move(vec), std::move(vec2)});
			return AddToPool(std::move(obj));
		}
		else
		{
			auto& lastPool = m_memory_pool[m_memory_pool.size() - 1];
			if (lastPool.dataBlock.size() < m_block_size)
			{
				auto ptr = rw_clustering_ptr{ &m_memory_pool, static_cast<unsigned int>(m_memory_pool.size() - 1), static_cast<unsigned int>(lastPool.dataBlock.size()) };
				lastPool.dataBlock.emplace_back(std::move(obj));
				return ptr;
			}
			else
			{
				auto vec = std::vector<T>();
				vec.reserve(m_block_size);
				auto vec2 = clustering_task_queue<T>(m_block_size);
				m_memory_pool.emplace_back(DataTaskBlockPair<T>{std::move(vec), std::move(vec2)});
				return AddToPool(std::move(obj));
			}
		}
	}
	
	void ExecuteClusters(int startId, int endId)
	{
		for (int i = startId; i < endId; ++i)
		{
			for (auto j = 0; j < m_memory_pool[i].taskQueue.size(); ++j)
			{
				m_memory_pool[i].taskQueue[j]();
			}
			m_memory_pool[i].taskQueue.clear();
		}
	}

	void ExecuteClusteredTasksParallel(lock_free_thread_pool& pool, bool isWaitTillFinish)
	{
		unsigned int poolSize = m_memory_pool.size();
		auto threadCount = std::min(poolSize, std::thread::hardware_concurrency());

		if (threadCount == 1)
		{
			ExecuteClusteredTasksSerial();
		}
		else
		{
			for (int i = 0; i < threadCount; ++i)
			{
				auto groupCount = poolSize / threadCount;
				auto startId = i * groupCount;
				auto endId = (i + 1) * groupCount;
				auto task = [startId = startId, endId = endId, this]() {std::invoke(&ClusteringMemoryPool<T>::ExecuteClusters, this, startId, endId); };
				pool.submit(task);
			}

			if(isWaitTillFinish)
			{
				pool.waitForAllTaskToFinish();
			}
		}
	}

	void ExecuteClusteredTasksSerial()
	{
		for (auto& a : m_memory_pool) 
		{
			for (auto j = 0; j < a.taskQueue.size(); ++j)
			{
				a.taskQueue[j]();
			}
			a.taskQueue.clear();
		}
	}

	~ClusteringMemoryPool() = default;

	std::vector<DataTaskBlockPair<T>> m_memory_pool;
private:
	unsigned int m_block_size;
};

#endif
