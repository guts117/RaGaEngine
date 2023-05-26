#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

#include <functional>

namespace detail
{
	template<size_t ExpectedSize, size_t ActualSize, size_t ExpectedAlignment, size_t ActualAlignment>
	inline void compare_size()
	{
		static_assert(ExpectedSize >= ActualSize, "The size for the ForwardDeclaredPimpl is wrong");
		static_assert(ExpectedAlignment >= ActualAlignment, "The alignment for the ForwardDeclaredPimpl is wrong");
	}
	template<size_t ExpectedSize, size_t ActualSize, size_t ExpectedAlignment, size_t ActualAlignment>
	struct size_comparer
	{
		inline size_comparer()
		{
			// going through one additional layer to get good error messages
			// if I put the assert down one more template layer, gcc will show the
			// sizes in the error message
			compare_size<ExpectedSize, ActualSize, ExpectedAlignment, ActualAlignment>();
		}
	};
}

struct forwarding_constructor {};

template<typename T, size_t Size, size_t Alignment = 16>
struct ForwardDeclaredPimpl
{
	ForwardDeclaredPimpl()
	{
		new (&Get()) T();
	}
	template<typename... Args>
	ForwardDeclaredPimpl(forwarding_constructor, Args &&... args)
	{
		new (&Get()) T(std::forward<Args>(args)...);
	}
	ForwardDeclaredPimpl(const ForwardDeclaredPimpl& other)
	{
		new (&Get()) T(other.Get());
	}
	ForwardDeclaredPimpl(const T& other)
	{
		new (&Get()) T(other);
	}
	ForwardDeclaredPimpl(ForwardDeclaredPimpl&& other)
	{
		new (&Get()) T(std::move(other.Get()));
	}
	ForwardDeclaredPimpl(T&& other)
	{
		new (&Get()) T(std::move(other));
	}
	ForwardDeclaredPimpl& operator=(const ForwardDeclaredPimpl& other)
	{
		Get() = other.Get();
		return *this;
	}
	ForwardDeclaredPimpl& operator=(const T& other)
	{
		Get() = other;
		return *this;
	}
	ForwardDeclaredPimpl& operator=(ForwardDeclaredPimpl&& other)
	{
		Get() = std::move(other.Get());
		return *this;
	}
	ForwardDeclaredPimpl& operator=(T&& other)
	{
		Get() = std::move(other);
		return *this;
	}
	~ForwardDeclaredPimpl()
	{
		detail::size_comparer<Size, sizeof(T), Alignment, alignof(T)> compare_size{};
		Get().~T();
	}
	T& Get()
	{
		return reinterpret_cast<T&>(*this);
	}
	const T& Get() const
	{
		return reinterpret_cast<const T&>(*this);
	}

private:
	alignas(Alignment) std::byte m_pImplBuff[Size];
};

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

struct funcBase;

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<funcBase> taskQueue;
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

struct funcBase
{
	virtual void Execute() {};
	alignas(alignof(void*)) std::byte buffer[alignof(void*) * 5];
};

template<typename memfn, typename forwarderArgs>
class funcWrapper : public funcBase
{
private:
	struct Impl
	{
		memfn&& func;
		forwarderArgs&& args;

		void Execute()
		{
			std::apply(std::forward<memfn>(func), std::forward<forwarderArgs>(args));
		}
	};

	const Impl* Pimpl() const { return buf; }
	Impl* Pimpl() { return buf; }

	Impl* buf;
public:
	funcWrapper(memfn&& func, forwarderArgs&& args) : buf { new (&buffer) Impl{ std::forward<memfn>(func), std::forward<forwarderArgs>(args) } }
	{
	}

	virtual void Execute() override
	{
		Pimpl()->Execute();
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

		//std::apply(func, forwarder{ ptr.get(), std::forward<Args>(args)... });
		auto forwarderArgs = std::move(forwarder{ ptr.get(), std::forward<Args>(args)... });
		auto defferedWrite = std::move(funcWrapper<decltype(func), decltype(forwarderArgs)>{ std::move(func), std::move(forwarderArgs) });
		//defferedWrite.Execute();
		//(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.push_back(std::mem_fn(&(decltype(defferedWrite)::Execute)));
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(std::move(defferedWrite));
		//(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(&(*ptr.poolHeadPtr)[ptr.clusterId].taskObjects.back());
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
				b.Execute();
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
