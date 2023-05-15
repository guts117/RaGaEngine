#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<std::function<void()>> taskQueue;
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
	friend class w_clustering_ptr<T>;
	friend class rw_clustering_ptr<T>;

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
	void write(memfn&& func, Args&&... args)
	{
		auto defferedWrite = [&]() {func(*ptr.get(), std::forward<Args>(args)...); };
		defferedWrite();
		//(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(std::move(defferedWrite));
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
			m_memory_pool.emplace_back(std::move(vec));
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
				m_memory_pool.emplace_back(std::move(vec));
				return AddToPool(std::move(obj));
			}
		}
	}
	
	void ExecuteClusteredTasks()
	{
		for (auto& a : m_memory_pool) 
		{
			for (auto b : a.taskQueue) 
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
