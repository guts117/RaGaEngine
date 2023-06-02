#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

#include <functional>
#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>

using namespace std;

//https://stackoverflow.com/questions/28509273/get-types-of-c-function-parameters
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<std::move_only_function<void()>> taskQueue;
};

template<class T>
struct rw_clustering_ptr;
template<class T>
struct funcWrapperToCluster;
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
		: clusterPtr { ptr }
		, func{ std::forward<memfn>(fn) }
	{
		new (Get()) Args(std::forward<Args>(args));
	}

	inline funcWrapper(const funcWrapper& other) noexcept
		: clusterPtr { other.clusterPtr }
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
		if(func != nullptr)
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
		auto args = std::make_tuple(inter.getThis(clusterPtr) );
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
			queue.emplace_back(std::move(funcWrapper(ptr, std::forward<memfn>(func), std::move(std::make_tuple(std::move(args)...)))));
		}
	}

	template<typename memfn>
	void stackingWrite(memfn&& func)
	{
		auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
		auto defferedWrite = [func = std::forward<memfn>(func), this]() { std::invoke(func, ptr.get()); };
		queue.emplace_back(std::move(defferedWrite));
	}

	//template<typename memfn, typename... Args>
	//void updateWrite (memfn&& func, Args&&... args)
	//{
	//	//ToDo: check whether all args are of the same reference type(i.e all lvalue or all rvalue)
	//	//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
	//	//(staticCheckForLvalue(std::forward<Args>(args)), ...);

	//	//byte* buf = ptr.get()->buffer;
	//	//byte testblock[sizeof(buf) / sizeof(*buf)];
	//	//memset(testblock, 0, sizeof(testblock));

	//	//ToDo: make a write that deletes existing funcWrapper and overwrites at the same place

	//	//if (!memcmp(testblock, buf, sizeof(buf) / sizeof(*buf)))
	//	//{
	//	//	auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
	//	//	auto defferedWrite = funcWrapper(ptr, std::forward<memfn>(func), std::move(std::make_tuple(std::move(args)...)));
	//	//	queue.emplace_back(std::move(defferedWrite));
	//	//}
	//	//else
	//	//{
	//	//}
	//}

	//template<typename memfn, typename... Args>
	//void stackingWrite(memfn&& func, Args&&... args)
	//{
	//	//ToDo: check whether all args are of the same reference type(i.e all lvalue or all rvalue)
	//	//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
	//	//(staticCheckForLvalue(std::forward<Args>(args)), ...);

	//	//byte* buf = ptr.get()->buffer;
	//	//byte testblock[sizeof(buf) / sizeof(*buf)];
	//	//memset(testblock, 0, sizeof(testblock));

	//	//ToDo: make a write that deletes existing funcWrapper and adds a new one at the end

	//	//if (!memcmp(testblock, buf, sizeof(buf) / sizeof(*buf)))
	//	//{
	//	//	auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
	//	//	auto defferedWrite = funcWrapper(ptr, std::forward<memfn>(func), std::move(std::make_tuple(std::move(args)...)));
	//	//	queue.emplace_back(std::move(defferedWrite));
	//	//}
	//	//else
	//	//{
	//	//}
	//}
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
			auto vec2 = std::vector<std::move_only_function<void()>>();
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
				auto vec2 = std::vector<std::move_only_function<void()>>();
				vec2.reserve(m_block_size);
				m_memory_pool.emplace_back(DataTaskBlockPair<T>{std::move(vec), std::move(vec2)});
				return AddToPool(std::move(obj));
			}
		}
	}
	
	void ExecuteClusteredTasks()
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
