#ifndef CLUSTERING_MEMORY_POOL
#define CLUSTERING_MEMORY_POOL

template<class T>
struct clustering_ptr
{
	std::vector<std::vector<T>>* poolHeadPtr = nullptr;
	unsigned int clusterId = 0;
	unsigned int index = 0;

	T* operator-> ()
	{
		return &((*poolHeadPtr)[clusterId][index]);
	}

	const T* get() const
	{
		return &((*poolHeadPtr)[clusterId][index]);
	}

	T* get()
	{
		return &((*poolHeadPtr)[clusterId][index]);
	}
};

template<class T>
class ClusteringMemoryPool
{
public:
	explicit ClusteringMemoryPool() = delete;
	explicit ClusteringMemoryPool(unsigned int block_size) : m_memory_pool {std::vector<std::vector<T>>()}, m_block_size{block_size}
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
			if (lastPool.size() < m_block_size)
			{
				auto ptr = clustering_ptr{ &m_memory_pool, static_cast<unsigned int>(m_memory_pool.size() - 1), static_cast<unsigned int>(lastPool.size()) };
				lastPool.emplace_back(std::move(obj));
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
	std::vector<std::vector<T>> m_memory_pool;
	unsigned int m_block_size;
};

#endif
