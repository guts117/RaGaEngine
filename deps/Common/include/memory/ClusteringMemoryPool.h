#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

template<class T>
struct DataTaskBlockPair
{
	std::vector<T> dataBlock;
	std::vector<std::function<void()>> taskQueue;
};

template<class T>
struct clustering_ptr
{
public:
	friend class w_clustering_ptr<T>;

	const T* operator-> ()
	{
		return &((*poolHeadPtr)[clusterId].dataBlock[index]);
	}

	const T* get() const
	{
		return &((*poolHeadPtr)[clusterId].dataBlock[index]);
	}

private:
	std::vector<DataTaskBlockPair<T>>* poolHeadPtr = nullptr;		//size 8
	unsigned int clusterId = 0;										//size 4
	unsigned int index = 0;											//size 4

	T* operator-> ()
	{
		return &((*poolHeadPtr)[clusterId].dataBlock[index]);
	}

	T* get()
	{
		return &((*poolHeadPtr)[clusterId].dataBlock[index]);
	}
};
template<class T>
struct r_clustering_ptr 
{
	const T* operator-> ()
	{
		return ptr.get();
	}

	const T* get()
	{
		return ptr.get();
	}

private:
	clustering_ptr<T> ptr;
};

template<class T>
struct w_clustering_ptr
{
private:
	T* operator-> ()
	{
		return ptr.get();
	}

	T* get()
	{
		return ptr.get();
	}
public:
	void write(T&& other)
	{
		auto defferedWrite = [&](T&& other) { *get() = std::move(other); };	
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(defferedWrite);;
	}

	#define DefferedWrite(func, args...) (w).write((w)->func(args...))

	void write()
	{
		DefferedWrite();
		(*ptr.poolHeadPtr)[ptr.clusterId].taskQueue.emplace_back(defferedWrite);;
	}
private:
	clustering_ptr<T> ptr;
};

template<class T>
class ClusteringMemoryPool
{
public:
	explicit ClusteringMemoryPool() = delete;
	explicit ClusteringMemoryPool(unsigned int block_size) : m_memory_pool {std::vector<DataTaskBlockPair<T>>()}, m_block_size{block_size}
	{
	}

	clustering_ptr<T> AddToPool(T&& obj)
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
				auto ptr = clustering_ptr{ &m_memory_pool, static_cast<unsigned int>(m_memory_pool.size() - 1), static_cast<unsigned int>(lastPool.dataBlock.size()) };
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
	
	~ClusteringMemoryPool() = default;
private:
	std::vector<DataTaskBlockPair<T>> m_memory_pool;
	unsigned int m_block_size;
};

#endif
