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

template<typename T>
class lock_based_threadsafe_queue
{
private:
	struct node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};
	std::mutex head_mutex;
	std::unique_ptr<node> head;
	std::mutex tail_mutex;
	node* tail;
	std::condition_variable data_cond;

	node* get_tail()
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		return tail;
	}
	std::unique_ptr<node> pop_head()                             
	{
		std::unique_ptr<node> old_head = std::move(head);
		head = std::move(old_head->next);
		return old_head;
	}
	std::unique_lock<std::mutex> wait_for_data()                 
	{
		std::unique_lock<std::mutex> head_lock(head_mutex);
		data_cond.wait(head_lock, [&] {return head.get() != get_tail(); });
		return std::move(head_lock);                             
	}
	std::unique_ptr<node> wait_pop_head()
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data()); 
			return pop_head();
	}
	std::unique_ptr<node> wait_pop_head(T& value)
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data()); 
			value = std::move(*head->data);
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if (head.get() == get_tail())
		{
			return std::unique_ptr<node>();
		}
		return pop_head();
	}
	std::unique_ptr<node> try_pop_head(T& value)
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if (head.get() == get_tail())
		{
			return std::unique_ptr<node>();
		}
		value = std::move(*head->data);
		return pop_head();
	}

public:
	lock_based_threadsafe_queue() :
		head(new node), tail(head.get())
	{
	}

	lock_based_threadsafe_queue(const lock_based_threadsafe_queue& rhs) noexcept = delete;
	lock_based_threadsafe_queue& operator=(const lock_based_threadsafe_queue& rhs) noexcept = delete;

	void push(T new_value)
	{
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		std::unique_ptr<node> p(new node);
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			tail->data = new_data;
			node* const new_tail = p.get();
			tail->next = std::move(p);
			tail = new_tail;
		}
		data_cond.notify_one();
	}

	std::shared_ptr<T> try_pop()
	{
		std::unique_ptr<node> old_head = try_pop_head();
		return old_head ? old_head->data : std::shared_ptr<T>();
	}
	bool try_pop(T& value)
	{
		std::unique_ptr<node> const old_head = try_pop_head(value);
		return old_head != nullptr;
	}
	bool empty()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		return (head.get() == get_tail());
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_ptr<node> const old_head = wait_pop_head();
		return old_head->data;
	}
	void wait_and_pop(T& value)
	{
		std::unique_ptr<node> const old_head = wait_pop_head(value);
	}
};

class lock_based_thread_pool
{
	std::atomic_bool done;
	lock_based_threadsafe_queue<std::function<void()>> work_queue;
	std::vector<std::jthread> threads;          

	void worker_thread()
	{
		while (!done)                                                       
		{
			std::function<void()> task;
			if (work_queue.try_pop(task))                                   
			{
				task();                                                    
			}
			else
			{
				std::this_thread::yield();                                 
			}
		}
	}
public:
	lock_based_thread_pool()
		: done(false)
	{
		unsigned const thread_count = std::thread::hardware_concurrency() - 1;   
		try
		{
			for (unsigned i = 0; i < thread_count; ++i)
			{
				threads.push_back(std::jthread(&lock_based_thread_pool::worker_thread, this));
			}
		}
		catch (...)
		{
			done = true;                                                     
				throw;
		}
	}
	~lock_based_thread_pool()
	{
		done = true;                                                         
	}

	bool isQueueEmpty() { return work_queue.empty(); }

	template<typename FunctionType>
	void submit(FunctionType f)
	{
		work_queue.push(std::function<void()>(f));                         
	}
};

class lock_free_thread_pool
{
	std::atomic_bool done;
	std::atomic_int fullQueueCount;
	vector<std::function<void()>> work_queue;
	std::vector<std::jthread> threads;

	void worker_thread(int threadIndex)
	{
		while (!done)
		{
			if (auto &task = work_queue[threadIndex])
			{
				task();
				task = nullptr;
				fullQueueCount.store(fullQueueCount - 1, memory_order::memory_order_release);
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
public:
	lock_free_thread_pool()
		: done(false)
		, fullQueueCount{0}
		, work_queue{ vector<std::function<void()>>(std::thread::hardware_concurrency())}
	{
		unsigned const thread_count = std::thread::hardware_concurrency();
		try
		{
			for (unsigned i = 0; i < thread_count; ++i)
			{
				threads.push_back(std::jthread(&lock_free_thread_pool::worker_thread, this, i));
			}
		}
		catch (...)
		{
			done = true;
			throw;
		}
	}
	~lock_free_thread_pool()
	{
		done = true;
	}

	template<typename FunctionType>
	void submit(FunctionType f)
	{
		while (fullQueueCount.load(memory_order::memory_order_acquire) >= std::thread::hardware_concurrency()) 
		{
			std::this_thread::yield;
		}
		work_queue[fullQueueCount] = f;
		fullQueueCount.store(fullQueueCount + 1, memory_order::memory_order_relaxed);
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
class move_only_function_32;

// Modified version of: https://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented
// Uses memcopy and memset so don't use for dynamically allocated members (can cause memory leaks)
// Warning / Rule: Don't use it to store function objects that have dynamically allocated members
template <typename R, typename... Args>
class move_only_function_32<R(Args...)>
{
	constexpr static auto BUFFER_SIZE = 32;
	constexpr static auto BUFFER_ALIGNMENT = 8;

	// function pointer types for the type-erasure behaviors
	// all these std::byte* parameters are actually casted from some functor type
	typedef R(*invoke_fn_t)(std::byte*, Args&&...);
	typedef void (*destroy_fn_t)(std::byte*);

	// type-aware generic functions for invoking
	// the specialization of these functions won't be capable with
	// the above function pointer types, so we need some cast
	template <typename Functor>
	static R invoke_fn(Functor* fn, Args&&... args)
	{
		return (*fn)(std::forward<Args>(args)...);
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
	move_only_function_32()
		: invoke_f(nullptr)
		, destroy_f(nullptr)
	{
		std::memset(this->data_ptr, 0x0, BUFFER_SIZE);
	}

	// construct from any functor type
	template <typename Functor>
	move_only_function_32(Functor&& f) noexcept
		// specialize functions and erase their type info by casting
		: invoke_f(reinterpret_cast<invoke_fn_t>(invoke_fn<Functor>))
		, destroy_f(reinterpret_cast<destroy_fn_t>(destroy_fn<Functor>))
	{
		// copy the functor to internal storage
		new (this->data_ptr) Functor(std::move(f));
	}

	move_only_function_32(move_only_function_32 const& rhs) noexcept = delete;
	move_only_function_32& operator= (move_only_function_32 const& rhs) = delete;

	move_only_function_32(move_only_function_32&& rhs) noexcept
		: invoke_f(std::exchange(rhs.invoke_f, nullptr))
		, destroy_f(std::exchange(rhs.destroy_f, nullptr))
	{
		if (this->invoke_f) {
			std::memcpy(this->data_ptr, rhs.data_ptr, BUFFER_SIZE);
			std::memset(rhs.data_ptr, 0x00, BUFFER_SIZE);
		}
	}

	move_only_function_32& operator=(move_only_function_32&& rhs) noexcept
	{
		invoke_f = std::exchange(rhs.invoke_f, nullptr);
		destroy_f = std::exchange(rhs.destroy_f, nullptr);

		if (this->invoke_f) {
			std::memcpy(this->data_ptr, rhs.data_ptr, BUFFER_SIZE);
			std::memset(rhs.data_ptr, 0x00, BUFFER_SIZE);
		}
		return *this;
	};

	~move_only_function_32() noexcept
	{
		if (destroy_f != nullptr) {
			this->destroy_f(this->data_ptr);
			std::memset(this->data_ptr, 0x00, BUFFER_SIZE);
		}
	}

	R operator()(Args&&... args)
	{
		return this->invoke_f(this->data_ptr, std::forward<Args>(args)...);
	}
};

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct clustering_ptr;

template<class T>
struct funcWrapperToCluster final
{
	std::byte* get(clustering_ptr<T>& ptr)
	{
		return ptr.get()->buffer;
	}
	std::byte const* const get(const clustering_ptr<T>& ptr) const
	{
		return ptr.get()->buffer;
	}

	T* getThis(clustering_ptr<T>& ptr) { return ptr.get(); }
	T const* const getThis(const clustering_ptr<T>& ptr) const { return ptr.get(); }
};

template<class T, typename memfn, typename Args>
struct funcWrapper final
{
	mutable clustering_ptr<T> clusterPtr;
	mutable memfn func;
	mutable funcWrapperToCluster<T> inter;

	inline funcWrapper(clustering_ptr<T>& ptr, memfn&& fn, Args&& args) noexcept
		: clusterPtr{ ptr }
		, func{ std::forward<memfn>(fn) }
	{
		new (Get()) Args(std::forward<Args>(args));
	}

	inline funcWrapper(const funcWrapper& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
	{
	}

	inline funcWrapper& operator=(const funcWrapper& other) noexcept
	{
		clusterPtr = other.clusterPtr;
		func = std::exchange(other.func, nullptr);
		return *this;
	}

	funcWrapper(funcWrapper&& other) noexcept
		: clusterPtr{ other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
	{
	}
	funcWrapper& operator=(funcWrapper&& other)
	{
		clusterPtr = other.clusterPtr;
		func = std::exchange(other.func, nullptr);
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
		if constexpr (std::is_rvalue_reference<decltype(std::get<1>(arguments(func)))>::value)
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

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<move_only_function_32<void()>> taskQueue;
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
	void oneTimeWrite(memfn&& func, Args&&... args)
	{
		//ToDo: check whether all args are of the same reference type(i.e all lvalue or all rvalue)
		//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
		//(staticCheckForLvalue(std::forward<Args>(args)), ...);

		byte* buf = ptr.get()->buffer;
		byte testblock[sizeof(buf) / sizeof(*buf)];
		memset(testblock, 0, sizeof(testblock));

		if (!memcmp(testblock, buf, (sizeof(buf) / sizeof(*buf))))
		{
			auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
			auto defferedWrite = funcWrapper(ptr, std::forward<memfn>(func), std::move(std::make_tuple(std::move(args)...)));
			queue.emplace_back(std::move(defferedWrite));
		}
	}

	//Fast push to the write task queue. Write to the same object can be stacked and will be logically cohorent
	//Sig: Accepts void()
	template<typename memfn>
	void stackingWrite(memfn&& func)
	{
		auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
		auto defferedWrite = [func = std::forward<memfn>(func), this]() { std::invoke(func, ptr.get()); };
		queue.emplace_back(std::move(defferedWrite));
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
			auto vec2 = std::vector<move_only_function_32<void()>>();
			vec2.reserve(m_block_size);
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
				auto vec2 = std::vector<move_only_function_32<void()>>();
				vec2.reserve(m_block_size);
				m_memory_pool.emplace_back(DataTaskBlockPair<T>{std::move(vec), std::move(vec2)});
				return AddToPool(std::move(obj));
			}
		}
	}
	
	void ExecuteClusters(int startId, int endId)
	{
		for (int i = startId; i < endId; ++i)
		{
			for (auto& a : m_memory_pool[i].taskQueue)
			{
				a();
			}
			m_memory_pool[i].taskQueue.clear();
		}
	}

	void ExecuteClusteredTasksParallel(lock_free_thread_pool& pool)
	{
		unsigned int poolSize = m_memory_pool.size();
		auto threadCount = std::min(poolSize, std::thread::hardware_concurrency());

		if (threadCount == 1)
		{
			ExecuteClusteredTasksSerial();
		}
		else
		{
			//std::function<void()> thisThreadTask;

			for (int i = 0; i < threadCount; ++i)
			{
				auto groupCount = poolSize / threadCount;
				auto startId = i * groupCount;
				auto endId = (i + 1) * groupCount;
				//if (i == 0)
				//{
				//	thisThreadTask = [=]() { ExecuteClusters(startId, endId); };
				//}
				//else
				//{
					auto yes = [startId = startId, endId = endId, this]() {std::invoke(&ClusteringMemoryPool<T>::ExecuteClusters, this, startId, endId); };
					pool.submit(yes);
				//}
			}

			//thisThreadTask();
		}
	}

	void ExecuteClusteredTasksSerial()
	{
		for (auto& a : m_memory_pool) 
		{
			for (auto& b : a.taskQueue) 
			{
				b();
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
