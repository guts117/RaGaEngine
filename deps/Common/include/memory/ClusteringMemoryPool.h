#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

#include <memory>
#include <iostream>
#include <vector>

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

template <typename T>
T& forwarder_type(T&);

// Here comes the deduction guide
template <typename... T>
forwarder(T&&... t) -> forwarder<decltype(forwarder_type(std::forward<T>(t)))...>;

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------


template <typename T>
class function;

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<function<void()>> taskQueue;
};

template<class T>
struct r_clustering_ptr;
template<class T>
struct w_clustering_ptr;
template<class T>
struct rw_clustering_ptr;

template<class T>
struct clustering_ptr
{
private:
	T* operator->()		{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
	T* get()			{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }

public:
	friend struct w_clustering_ptr<T>;
	friend struct rw_clustering_ptr<T>;

	//ToDo: Write proper constructors

	std::vector<DataTaskBlockPair<T>>* poolHeadPtr = nullptr;		//size 8
	unsigned int clusterId = 0;										//size 4
	unsigned int index = 0;											//size 4

	T const* const operator-> () const	{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
	T const* const get() const			{ return &((*poolHeadPtr)[clusterId].dataBlock[index]); }
};
template<class T>
struct r_clustering_ptr 
{
private:
	clustering_ptr<T> ptr;

public:
	//ToDo: Write proper constructors
	T const* const operator-> () const	{ return ptr.get(); }
	T const* const get() const			{ return ptr.get(); }
	bool isValid()		const			{ return ptr.poolHeadPtr != nullptr; }
};

template<class T>
struct w_clustering_ptr
{
private:
	clustering_ptr<T> ptr;

public:
	//ToDo: Write proper constructors
	bool isValid()	const				{ return ptr.poolHeadPtr != nullptr; }

	void write(T&& other)
	{
		auto defferedWrite = [&](T&& other) { *ptr.get() = std::move(other); };
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(defferedWrite);
	}

	template<typename memfn, typename... Args>
	void write(memfn&& func, Args&&... args)
	{
		auto defferedWrite = [&]() {func(*ptr.get(), std::forward<Args>(args)...); };
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(defferedWrite);
	}
};

template <typename R, typename... Args>
class function<R(Args...)>
{
	// function pointer types for the type-erasure behaviors
	// all these char* parameters are actually casted from some functor type
	typedef R(*invoke_fn_t)(char*, Args&&...);
	typedef void (*construct_fn_t)(char*, char*);
	typedef void (*destroy_fn_t)(char*);

	// type-aware generic functions for invoking
	// the specialization of these functions won't be capable with
	//   the above function pointer types, so we need some cast
	template <typename Functor>
	static R invoke_fn(Functor* fn, Args&&... args)
	{
		return (*fn)(std::forward<Args>(args)...);
	}

	template <typename Functor>
	static void construct_fn(Functor* construct_dst, Functor* construct_src)
	{
		// the functor type must be copy-constructible
		new (construct_dst) Functor(*construct_src);
	}

	template <typename Functor>
	static void destroy_fn(Functor* f)
	{
		f->~Functor();
	}

	// these pointers are storing behaviors
	invoke_fn_t invoke_f;
	construct_fn_t construct_f;
	destroy_fn_t destroy_f;

	// erase the type of any functor and store it into a char*
	// so the storage size should be obtained as well
	mutable alignas(8) char data_ptr[8*14];
public:
	function()
		: invoke_f(nullptr)
		, construct_f(nullptr)
		, destroy_f(nullptr)
	{}

	// construct from any functor type
	template <typename Functor>
	function(Functor f)
		// specialize functions and erase their type info by casting
		: invoke_f(reinterpret_cast<invoke_fn_t>(invoke_fn<Functor>))
		, construct_f(reinterpret_cast<construct_fn_t>(construct_fn<Functor>))
		, destroy_f(reinterpret_cast<destroy_fn_t>(destroy_fn<Functor>))
	{
		// copy the functor to internal storage
		this->construct_f(this->data_ptr, reinterpret_cast<char*>(&f));
	}

	// copy constructor
	function(function const& rhs)
		: invoke_f(rhs.invoke_f)
		, construct_f(rhs.construct_f)
		, destroy_f(rhs.destroy_f)
	{
		if (this->invoke_f) {
			this->construct_f(this->data_ptr, rhs.data_ptr);
		}
	}

	~function()
	{
		if (data_ptr != nullptr) {
			this->destroy_f(this->data_ptr);
		}
	}

	// other constructors, from nullptr, from function pointers

	R operator()(Args&&... args)
	{
		return this->invoke_f(this->data_ptr, std::forward<Args>(args)...);
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

//https://stackoverflow.com/questions/2821223/how-would-one-call-stdforward-on-all-arguments-in-a-variadic-function
	template<typename memfn, typename... Args>
	void shallowWrite(memfn&& func, Args&&... args)
	{
		//std::invoke(std::forward<memfn>(func), ptr.get(), std::forward<Args>(args)...);
		
		auto defferedWrite = [func = std::move(func), args..., this]() mutable
		{
			std::invoke(std::forward<memfn>(func), ptr.get(), std::forward<Args>(args)...);
		};
		//defferedWrite();
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(std::move(defferedWrite));
	}

//ToDo: Capture by move thingy https://marcoarena.wordpress.com/2012/11/01/learn-how-to-capture-by-move/
	template<typename memfn, typename... Args>
	void write(memfn&& func, Args&&... args)
	{
		auto staticCheckForLvalue = [] <typename packArg> (packArg&&) mutable { static_assert(!std::is_lvalue_reference<packArg>::value, "Has lvalue reference please change"); };

		(staticCheckForLvalue(std::forward<Args>(args)), ...);

		////std::apply(func, forwarder{ ptr.get(), std::forward<Args>(args)... });
		//auto defferedWrite = [func = std::move(func), args = std::move(forwarder{ ptr.get(), std::forward<Args>(args)... })]() mutable
		//{
		//	std::apply(std::move(func), std::move(args));
		//};
		////defferedWrite();
		//(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(std::move(defferedWrite));

		//std::apply(func, forwarder{ ptr.get(), args ... });
		auto defferedWrite = [func = std::move(func), args = forwarder{ ptr.get(), args... }]() mutable
		{
			std::apply(std::move(func), std::move(args));
		};
		//defferedWrite();
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(defferedWrite);
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
private:
	std::vector<DataTaskBlockPair<T>> m_memory_pool;
	unsigned int m_block_size;
};

#endif
