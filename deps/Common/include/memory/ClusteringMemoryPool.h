#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

#include <functional>
#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>

using namespace std;

//https://vittorioromeo.info/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html
//https://stackoverflow.com/questions/26831382/capturing-perfectly-forwarded-variable-in-lambda
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

// This is the generic case
template <typename... T>
struct forwarder : public std::tuple<T...> {
	using std::tuple<T...>::tuple;
};

// This is the case when just one variable is being captured.
template <typename T>
struct forwarder<T> : public std::tuple<T> {
	using std::tuple<T>::tuple;

	// Pointer-like accessors
	auto& operator *() {
		return std::get<0>(*this);
	}

	const auto& operator *() const {
		return std::get<0>(*this);
	}

	auto* operator ->() {
		return &std::get<0>(*this);
	}

	const auto* operator ->() const {
		return &std::get<0>(*this);
	}
};

// std::tuple_size needs to be specialized for our type, 
// so that std::apply can be used.
namespace std {
	template <typename... T>
	struct tuple_size<forwarder<T...>> : tuple_size<tuple<T...>> {};
}

// The below two functions declarations are used by the deduction guide
// to determine whether to copy or reference the variable
template <typename T>
T forwarder_type(const T&);

//template <typename T>
//T& forwarder_type(T&);

// Here comes the deduction guide
template <typename... T>
forwarder(T&&... t) -> forwarder<decltype(forwarder_type(std::forward<T>(t)))...>;

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<std::function<void()>> taskQueue;
};

template<class T>
struct rw_clustering_ptr;
template<class T>
struct funcWrapperBase;
template<class T>
struct ClusteringMemoryPool;

template<class T>
struct clustering_ptr
{
private:
	inline T* operator->()		{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
	inline T* get()			{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }

public:
	friend struct rw_clustering_ptr<T>;
	friend struct funcWrapperBase<T>;

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

template<class T>
struct funcWrapperBase 
{
virtual ~funcWrapperBase() noexcept = 0 {}

protected:
	std::byte* get(clustering_ptr<T>& ptr) { return ptr.get()->buffer; }
	std::byte const* const get(const clustering_ptr<T>& ptr) const { return ptr.get()->buffer; }

	T* getThis(clustering_ptr<T>& ptr) { return ptr.get(); }
	T const* const getThis(const clustering_ptr<T>& ptr) const { return ptr.get(); }
};

template<class T, typename memfn, typename Args>
struct funcWrapper final : public funcWrapperBase<T>
{
	mutable clustering_ptr<T> clusterPtr;
	mutable memfn func;

	inline funcWrapper(clustering_ptr<T>& ptr, memfn&& fn, Args&& args) noexcept
		: clusterPtr { ptr }
		, func{ std::forward<memfn>(fn) }
	{
		new (&Get()) Args(std::forward<Args>(args));
	}

	inline funcWrapper(const funcWrapper& other) noexcept
		: clusterPtr { other.clusterPtr }
		, func{ std::exchange(other.func, nullptr) }
	{	
	}

	inline funcWrapper& operator=(const funcWrapper& other) noexcept
	{
		funcWrapperBase<T>::operator=(other);
		func = std::exchange(other.func, nullptr);
		return *this;
	}

	funcWrapper(funcWrapper&& other) noexcept = delete;
	funcWrapper& operator=(funcWrapper&& other) = delete;

	inline ~funcWrapper() noexcept
	{
		if(func != nullptr)
		{
			Get().~Args();
			func = nullptr;
		}
	}

	inline Args& Get() noexcept
	{
		return reinterpret_cast<Args&>(*this->get(clusterPtr));
	}

	inline const Args& Get() const noexcept
	{
		return reinterpret_cast<const Args&>(*this->get(clusterPtr));
	}

	inline void operator()() noexcept
	{
		auto args = std::make_tuple( this->getThis(clusterPtr) );
		auto tupArgs = std::tuple_cat(args, std::move(Get()));
		//std::invoke(func, this->getThis(), Get());
		std::apply(func, tupArgs);
		//if constexpr (std::is_rvalue_reference<decltype(std::get<1>(arguments(func)))>::value) //works for void(T) and void (T&&)
		//{
		//	std::apply(std::move(func), std::move(Get()));
		//}
		//else //works for void(T&)
		//{
		//	std::apply(func, Get());
		//}
	}
};

template<class T>
struct rw_clustering_ptr
{
private:
	clustering_ptr<T> ptr;

public:
	explicit rw_clustering_ptr() = default;
	explicit rw_clustering_ptr(std::vector<DataTaskBlockPair<T>>* poolHeadPtr, unsigned int clusterId, unsigned int index) : ptr{ poolHeadPtr, clusterId, index} {}

	T const* const operator-> () const	{ return ptr.get(); }
	T const* const get() const			{ return ptr.get(); }
	bool isValid() const				{ return ptr.poolHeadPtr != nullptr; }

	void write(T&& other)
	{
		auto defferedWrite = [&](T&& other) { *ptr.get() = std::move(other); };
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(defferedWrite);
	}

	template<typename memfn, typename... Args>
	void oneTimeWrite(memfn&& func, Args&&... args)
	{
		//This is here for two reasons:
		// 1. To discourage people from using lvalue references.
		// 2. It works without this cuz forwwarder will copy everything,
		//	  but my tests show that copying is faster due to compiler optimizations.
		// 3. this function will be working mosting with aggregrate classes and primitive values
		//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
		//(staticCheckForLvalue(std::forward<Args>(args)), ...);

		//auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;

		//using type = funcWrapper<std::decay<memfn>::type, forwarder<std::decay<decltype(ptr.get())>::type, std::decay<Args>::type...>>;

		//auto ys = std::find_if(queue.begin(), queue.end(), [&](auto& fw) -> bool { return fw.target<type>()->isSame(func, ptr.index); });
		//if (ys != std::end(queue))
		//{
		//	return;
		//}
		//else
		//{
		//	auto defferedWrite = funcWrapper(std::move(func), std::move(forwarder{ ptr.get(), std::move(args)...}), ptr.index);
		//	//defferedWrite();
		//	queue.emplace_back(std::move(defferedWrite));
		//}
	}

	template<typename memfn, typename... Args>
	void updateWrite (memfn&& func, Args&&... args)
	{
		//This is here for two reasons:
		// 1. To discourage people from using lvalue references.
		// 2. It works without this cuz forwwarder will copy everything,
		//	  but my tests show that copying is faster due to compiler optimizations.
		// 3. this function will be working mosting with aggregrate classes and primitive values
		//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
		//(staticCheckForLvalue(std::forward<Args>(args)), ...);

		//auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;

		//using type = funcWrapper<std::decay<memfn>::type, forwarder<std::decay<decltype(ptr.get())>::type, std::decay<Args>::type...>>;
		//
		//auto ys = std::find_if(queue.begin(), queue.end(), [&](auto& fw) -> bool { return fw.target<type>()->isSame(func, ptr.index); });
		//if (ys != std::end(queue))
		//{
		//	auto defferedWrite = funcWrapper(std::move(func), std::move(forwarder{ ptr.get(), std::move(args)... }), ptr.index);
		//	*ys = std::move(defferedWrite);
		//}
		//else
		//{
		//	auto defferedWrite = funcWrapper(std::move(func), std::move(forwarder{ ptr.get(), std::move(args)... }), ptr.index);
		//	queue.emplace_back(std::move(defferedWrite));
		//}
	}

	//Warning: Will cause running out of memory problem
	template<typename memfn, typename... Args>
	void stackingWrite(memfn&& func, Args&&... args)
	{
		//This is here for two reasons:
		// 1. To discourage people from using lvalue references.
		// 2. It works without this cuz forwwarder will copy everything,
		//	  but my tests show that copying is faster due to compiler optimizations.
		// 3. this function will be working mosting with aggregrate classes and primitive values
		//auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };
		//(staticCheckForLvalue(std::forward<Args>(args)), ...);

		auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
		auto defferedWrite = funcWrapper(ptr, std::forward<memfn>(func), std::move(std::make_tuple(std::move(args)... )));
		//defferedWrite();
		queue.emplace_back(defferedWrite);
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
			m_memory_pool.emplace_back(DataTaskBlockPair<T>{std::move(vec)});
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
				m_memory_pool.emplace_back(DataTaskBlockPair<T>{std::move(vec)});
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
