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


#include <cassert>
#include <cstdlib>
#include <cstring> // std::memcpy()
#include <new>
#include <type_traits>
#include <iterator>
#include <utility>
#include <stdexcept>

/**
 * @file uvector.h
 * Header file for uvector and its relational and swap functions.
 * @author André Offringa
 * @copyright André Offringa, 2013, distributed under the GPL license version 3.
 */

namespace ao {

	/**
	 * @defgroup uvector Class uvector and related functions.
	 * @{
	 */

	 /**
	  * @brief A container similar to std::vector, but one that allows construction without initializing its elements.
	  * @details This container is similar to a std::vector, except that it can be constructor without
	  * initializing its elements. This saves the overhead of initialization, hence the
	  * constructor @ref uvector(size_t) is significantly faster than the corresponding std::vector
	  * constructor, and has no overhead to a manually allocated array.
	  *
	  * Probably its greatest strength lies in the construction of containers with a number of elements
	  * that is runtime defined, but that will be initialized later. For example:
	  *
	  * @code
	  * // Open a file
	  * ifstream file("myfile.bin");
	  *
	  * // Construct a buffer for this file
	  * uvector<char> buffer(buffer_size);
	  *
	  * // Read some data into the buffer
	  * file.read(&buffer[0], buffer_size);
	  * @endcode
	  *
	  * However, it has a few more use-cases with improved performance over std::vector. This is
	  * true because of more strengent requirements on the element's type.
	  *
	  * The container will behave correctly with any trivial type, but will not work for almost
	  * all non-trivial types.
	  *
	  * The element type must be trivial. Because of the use of @c memcpy and @c memmove,
	  * the @ref push_back() and @ref insert() methods are a bit faster than the std::vector
	  * counterparts, at least on gcc 4.7.
	  *
	  * The methods with different semantics compared to std::vector are:
	  * * @ref uvector(size_t n)
	  * * @ref resize(size_t n)
	  *
	  * Also the following new members are introduced:
	  * * @ref insert_uninitialized(const_iterator position, size_t n)
	  * * @ref push_back(InputIterator first, InputIterator last)
	  * * @ref push_back(size_t n, const Tp& val)
	  * * @ref push_back(std::initializer_list<Tp> initlist)
	  * * @ref push_back_uninitialized(size_t n)
	  *
	  * All other members work exactly like std::vector's members, although some are slightly faster because of
	  * the stricter requirements on the element type.
	  *
	  * @tparam Tp Container's element type
	  * @tparam Alloc Allocator type. Default is to use the std::allocator.
	  *
	  * @author André Offringa
	  * @copyright André Offringa, 2013, distributed under the GPL license version 3.
	  */
	template<typename Tp, typename Alloc = std::allocator<Tp> >
	class uvector : private Alloc
	{
		static_assert(std::is_standard_layout<Tp>(), "A uvector can only hold classes with standard layout");
	private:
#if __cplusplus > 201402L
		typedef std::allocator_traits<allocator_type>::is_always_equal allocator_is_always_equal;
#else
		typedef std::false_type allocator_is_always_equal;
#endif
	public:
		/// Element type
		typedef Tp value_type;
		/// Type of allocator used to allocate and deallocate space
		typedef Alloc allocator_type;
		/// Reference to element type
		typedef Tp& reference;
		/// Constant reference to element type
		typedef const Tp& const_reference;
		/// Pointer to element type
		typedef Tp* pointer;
		/// Pointer to constant element type
		typedef const Tp* const_pointer;
		/// Iterator type
		typedef Tp* iterator;
		/// Iterator type of constant elements
		typedef const Tp* const_iterator;
		/// Reverse iterator type
		typedef std::reverse_iterator<iterator> reverse_iterator;
		/// Reverse iterator of constant elements
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		/// Difference between to iterators
		typedef std::ptrdiff_t difference_type;
		/// Type used for indexing elements
		typedef std::size_t size_t;
		/// Type used for indexing elements
		typedef std::size_t size_type;

	private:
		pointer _begin, _end, _endOfStorage;

	public:
		/** @brief Construct an empty uvector.
		 * @param allocator Allocator used for allocating and deallocating memory.
		 */
		explicit uvector(const allocator_type& allocator = Alloc()) noexcept
			: Alloc(allocator), _begin(nullptr), _end(nullptr), _endOfStorage(nullptr)
		{
		}

		/** @brief Construct a vector with given amount of elements, without initializing these.
		 * @details This constructor deviates from std::vector's behaviour, because it will not
		 * value construct its elements. It is therefore faster than the corresponding constructor
		 * of std::vector.
		 * @param n Number of elements that the uvector will be initialized with.
		 */
		explicit uvector(size_t n) :
			_begin(allocate(n)),
			_end(_begin + n),
			_endOfStorage(_end)
		{
		}

		/** @brief Construct a vector with given amount of elements and set these to a specific value.
		 * @details This constructor will initialize its members with the given value.
		 * @param n Number of elements that the uvector will be initialized with.
		 * @param val Value to initialize all elements with
		 * @param allocator Allocator used for allocating and deallocating memory.
		 */
		uvector(size_t n, const value_type& val, const allocator_type& allocator = Alloc()) :
			Alloc(allocator),
			_begin(allocate(n)),
			_end(_begin + n),
			_endOfStorage(_end)
		{
			std::uninitialized_fill_n<Tp*, size_t>(_begin, n, val);
		}

		/** @brief Construct a vector by copying elements from a range.
		 * @param first Iterator to range start
		 * @param last Iterator to range end
		 * @param allocator Allocator used for allocating and deallocating memory.
		 */
		template<class InputIterator>
		uvector(InputIterator first, InputIterator last, const allocator_type& allocator = Alloc()) :
			Alloc(allocator)
		{
			construct_from_range<InputIterator>(first, last, std::is_integral<InputIterator>());
		}

		/** @brief Copy construct a uvector.
		 * @details The allocator of the new uvector will be initialized from
		 * @c std::allocator_traits<Alloc>::select_on_container_copy_construction(other).
		 * @param other Source uvector to be copied from.
		 */
		uvector(const uvector<Tp, Alloc>& other) :
			Alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(static_cast<allocator_type>(other))),
			_begin(allocate(other.size())),
			_end(_begin + other.size()),
			_endOfStorage(_end)
		{
			memcpy(_begin, other._begin, other.size() * sizeof(Tp));
		}

		/** @brief Copy construct a uvector with custom allocator.
		 * @param other Source uvector to be copied from.
		 * @param allocator Allocator used for allocating and deallocating memory.
		 */
		uvector(const uvector<Tp, Alloc>& other, const allocator_type& allocator) :
			Alloc(allocator),
			_begin(allocate(other.size())),
			_end(_begin + other.size()),
			_endOfStorage(_end)
		{
			memcpy(_begin, other._begin, other.size() * sizeof(Tp));
		}

		/** @brief Move construct a uvector.
		 * @param other Source uvector to be moved from.
		 */
		uvector(uvector<Tp, Alloc>&& other) noexcept :
			Alloc(std::move(other)),
			_begin(other._begin),
			_end(other._end),
			_endOfStorage(other._endOfStorage)
		{
			other._begin = nullptr;
			other._end = nullptr;
			other._endOfStorage = nullptr;
		}

		/** @brief Move construct a uvector with custom allocator.
		 * @param other Source uvector to be moved from.
		 * @param allocator Allocator used for allocating and deallocating memory.
		 */
		uvector(uvector<Tp, Alloc>&& other, const allocator_type& allocator) noexcept :
			Alloc(allocator),
			_begin(other._begin),
			_end(other._end),
			_endOfStorage(other._endOfStorage)
		{
			other._begin = nullptr;
			other._end = nullptr;
			other._endOfStorage = nullptr;
		}

		/** @brief Construct a uvector from a initializer list.
		 * @param initlist Initializer list used for initializing the new uvector.
		 * @param allocator Allocator used for allocating and deallocating memory.
		 */
		uvector(std::initializer_list<Tp> initlist, const allocator_type& allocator = Alloc()) :
			Alloc(allocator),
			_begin(allocate(initlist.size())),
			_end(_begin + initlist.size()),
			_endOfStorage(_end)
		{
			iterator destIter = _begin;
			for (typename std::initializer_list<Tp>::const_iterator i = initlist.begin(); i != initlist.end(); ++i)
			{
				*destIter = *i;
				++destIter;
			}
		}

		/** @brief Destructor. */
		~uvector() noexcept
		{
			deallocate();
		}

		/** @brief Assign another uvector to this uvector.
		 * @details The allocator of the uvector will be assigned to @p other when
		 * std::allocator_traits<Alloc>::propagate_on_container_copy_assignment() is of true_type.
		 */
		uvector& operator=(const uvector<Tp, Alloc>& other)
		{
			return assign_copy_from(other, typename std::allocator_traits<Alloc>::propagate_on_container_copy_assignment());
		}

		/** @brief Assign another uvector to this uvector.
		 * @details The allocator of the uvector will be assigned to @p other when
		 * std::allocator_traits<Alloc>::propagate_on_container_move_assignment() is of true_type.
		 */
		uvector& operator=(uvector<Tp, Alloc>&& other) noexcept(
			std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value ||
			allocator_is_always_equal::value)
		{
			return assign_move_from(std::move(other), typename std::allocator_traits<Alloc>::propagate_on_container_move_assignment());
		}

		/** @brief Get iterator to first element. */
		iterator begin() noexcept { return _begin; }

		/** @brief Get constant iterator to first element. */
		const_iterator begin() const noexcept { return _begin; }

		/** @brief Get iterator to element past last element. */
		iterator end() noexcept { return _end; }

		/** @brief Get constant iterator to element past last element. */
		const_iterator end() const noexcept { return _end; }

		/** @brief Get reverse iterator to last element. */
		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

		/** @brief Get constant reverse iterator to last element. */
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

		/** @brief Get reverse iterator to element before first element. */
		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

		/** @brief Get constant reverse iterator to element before first element. */
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

		/** @brief Get constant iterator to first element. */
		const_iterator cbegin() const noexcept { return _begin; }

		/** @brief Get constant iterator to element past last element. */
		const_iterator cend() const noexcept { return _end; }

		/** @brief Get constant reverse iterator to last element. */
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

		/** @brief Get constant reverse iterator to element before first element. */
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

		/** @brief Get number of elements in container. */
		size_t size() const noexcept { return _end - _begin; }

		/** @brief Get maximum number of elements that this container can hold. */
		size_t max_size() const noexcept { return Alloc::max_size(); }

		/** @brief Change the number of elements in the container.
		 * @details If the new size is larger than the current size, new values will be
		 * left uninitialized. Therefore, it is more efficient than @c resize(size_t) in
		 * @c std::vector, as well as @ref resize(size_t, const Tp&).
		 * If the new size is smaller than the current size, the container will be
		 * truncated and elements past the new size will be removed. No destructor of the
		 * removed elements will be called.
		 * @param n The new size of the container.
		 */
		void resize(size_t n)
		{
			if (capacity() < n)
			{
				size_t newSize = enlarge_size(n);
				pointer newStorage = allocate(newSize);
				memcpy(newStorage, _begin, size() * sizeof(Tp));
				deallocate();
				_begin = newStorage;
				_endOfStorage = _begin + newSize;
			}
			_end = _begin + n;
		}

		/** @brief Change the number of elements in the container.
		 * @details If the new size is larger than the current size, new values will be
		 * initialized by the given value.
		 * If the new size is smaller than the current size, the container will be
		 * truncated and elements past the new size will be removed. No destructor of the
		 * removed elements will be called.
		 * @param n The new size of the container.
		 * @param val New value of elements that get added to the container.
		 */
		void resize(size_t n, const Tp& val)
		{
			size_t oldSize = size();
			if (capacity() < n)
			{
				pointer newStorage = allocate(n);
				memcpy(newStorage, _begin, size() * sizeof(Tp));
				deallocate();
				_begin = newStorage;
				_endOfStorage = _begin + n;
			}
			_end = _begin + n;
			if (oldSize < n)
				std::uninitialized_fill<Tp*, size_t>(_begin + oldSize, _end, val);
		}

		/** @brief Get the number of elements the container can currently hold without reallocating storage. */
		size_t capacity() const noexcept { return _endOfStorage - _begin; }

		/** @brief Determine if the container is currently empty.
		 * @returns @c true if @ref size() == 0. */
		bool empty() const noexcept { return _begin == _end; }

		/** @brief Reserve space for a number of elements, to prevent the overhead of extra
		 * reallocations.
		 * @details This has no effect on the working of the uvector, except that it might change
		 * the current capacity. This can enhance performance when a large number of elements are added,
		 * and an approximate size is known a priori.
		 *
		 * This method might cause a reallocation, causing iterators to be invalidated.
		 * @param n Number of elements to reserve space for.
		 */
		void reserve(size_t n)
		{
			if (capacity() < n)
			{
				const size_t curSize = size();
				pointer newStorage = allocate(n);
				memcpy(newStorage, _begin, curSize * sizeof(Tp));
				deallocate();
				_begin = newStorage;
				_end = newStorage + curSize;
				_endOfStorage = _begin + n;
			}
		}

		/** @brief Change the capacity of the container such that no extra space is hold.
		 * @details This has no effect on the working of the uvector, except that it might change
		 * the current capacity. This can reduce the current memory usage of the container.
		 *
		 * This method might cause a reallocation, causing iterators to be invalidated.
		 */
		void shrink_to_fit()
		{
			const size_t curSize = size();
			if (curSize == 0)
			{
				deallocate();
				_begin = nullptr;
				_end = nullptr;
				_endOfStorage = nullptr;
			}
			else if (curSize < capacity()) {
				pointer newStorage = allocate(curSize);
				memcpy(newStorage, _begin, curSize * sizeof(Tp));
				deallocate();
				_begin = newStorage;
				_end = newStorage + curSize;
				_endOfStorage = _begin + curSize;
			}
		}

		/** @brief Get a reference to the element at the given index. */
		Tp& operator[](size_t index) noexcept { return _begin[index]; }

		/** @brief Get a constant reference to the element at the given index. */
		const Tp& operator[](size_t index) const noexcept { return _begin[index]; }

		/** @brief Get a reference to the element at the given index with bounds checking.
		 * @throws std::out_of_range when given index is past the last element.
		 */
		Tp& at(size_t index)
		{
			check_bounds(index);
			return _begin[index];
		}

		/** @brief Get a constant reference to the element at the given index with bounds checking.
		 * @throws std::out_of_range when given index is past the last element.
		 */
		const Tp& at(size_t index) const
		{
			check_bounds(index);
			return _begin[index];
		}

		/** @brief Get reference to first element in container. */
		Tp& front() noexcept { return *_begin; }

		/** @brief Get constant reference to first element in container. */
		const Tp& front() const noexcept { return *_begin; }

		/** @brief Get reference to last element in container. */
		Tp& back() noexcept { return *(_end - 1); }

		/** @brief Get constant reference to last element in container. */
		const Tp& back() const noexcept { return *(_end - 1); }

		/** @brief Get pointer to internal storage. */
		Tp* data() noexcept { return _begin; }

		/** @brief Get constant pointer to internal storage. */
		const Tp* data() const noexcept { return _begin; }

		/** @brief Assign this container to be equal to the given range.
		 * @details The container will be resized to fit the length of the given
		 * range. Iterators are invalidated.
		 * @param first Iterator to the beginning of the range.
		 * @param last Iterator past the end of the range.
		 */
		template<class InputIterator>
		void assign(InputIterator first, InputIterator last)
		{
			assign_from_range<InputIterator>(first, last, std::is_integral<InputIterator>());
		}

		/** @brief Resize the container and assign the given value to all elements.
		 * @details Iterators are invalidated.
		 * @param n New size of container
		 * @param val Value to be assigned to all elements.
		 */
		void assign(size_t n, const Tp& val)
		{
			if (n > capacity())
			{
				iterator newStorage = allocate(n);
				deallocate();
				_begin = newStorage;
				_endOfStorage = _begin + n;
			}
			_end = _begin + n;
			std::uninitialized_fill_n<Tp*, size_t>(_begin, n, val);
		}

		/** @brief Assign this container to an initializer list.
		 * @details The container will be resized to fit the length of the given
		 * initializer list. Iterators are invalidated.
		 * @param initlist List of values to assign to the container.
		 */
		void assign(std::initializer_list<Tp> initlist)
		{
			if (initlist.size() > capacity())
			{
				iterator newStorage = allocate(initlist.size());
				deallocate();
				_begin = newStorage;
				_endOfStorage = _begin + initlist.size();
			}
			_end = _begin + initlist.size();
			iterator destIter = _begin;
			for (typename std::initializer_list<Tp>::const_iterator i = initlist.begin(); i != initlist.end(); ++i)
			{
				*destIter = *i;
				++destIter;
			}
		}

		/** @brief Add the given value to the end of the container.
		 * @details Iterators are invalidated.
		 * @param item Value of new element.
		 */
		void push_back(const Tp& item)
		{
			if (_end == _endOfStorage)
				enlarge(enlarge_size(1));
			*_end = item;
			++_end;
		}

		/** @brief Add the given value to the end of the container by moving it in.
		 * @details Iterators are invalidated.
		 *
		 * Note that this container can only hold simple types that do not perform allocations. Therefore,
		 * there is probably no benefit in moving the new item in over copying it in with @ref push_back(const Tp&).
		 * @param item Value of new element.
		 */
		void push_back(Tp&& item)
		{
			if (_end == _endOfStorage)
				enlarge(enlarge_size(1));
			*_end = std::move(item);
			++_end;
		}

		/** @brief Remove the last element from the container. */
		void pop_back()
		{
			--_end;
		}

		/** @brief Insert an element at a given position.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive.
		 * @param position Position of the new element. The new element will be added before the old element
		 * at that position.
		 * @param item Value of the new item.
		 * @return Position of the new element.
		 */
		iterator insert(const_iterator position, const Tp& item)
		{
			if (_end == _endOfStorage)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(1), index, 1);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + 1, position, (_end - position) * sizeof(Tp));
				++_end;
			}
			*const_cast<iterator>(position) = item;
			return const_cast<iterator>(position);
		}

		/** @brief Insert elements at a given position and initialize them with a value.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive.
		 * @param position Position of the new elements. The new elements will be added before the old element
		 * at that position.
		 * @param n Number of elements to add.
		 * @param val Value of the new item.
		 * @return Position of the first new element.
		 */
		iterator insert(const_iterator position, size_t n, const Tp& val)
		{
			if (capacity() < size() + n)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(n), index, n);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + n, position, (_end - position) * sizeof(Tp));
				_end += n;
			}
			std::uninitialized_fill_n<Tp*, size_t>(const_cast<iterator>(position), n, val);
			return const_cast<iterator>(position);
		}

		/** @brief Insert elements at a given position and initialize them from a range.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive.
		 * @param position Position of the new elements. The new elements will be added before the old element
		 * at that position.
		 * @param first Iterator to the beginning of the range.
		 * @param last Iterator past the end of the range.
		 * @return Position of the first new element.
		 */
		template <class InputIterator>
		iterator insert(const_iterator position, InputIterator first, InputIterator last)
		{
			return insert_from_range<InputIterator>(position, first, last, std::is_integral<InputIterator>());
		}

		/** @brief Insert an element at a given position by moving it in.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive.
		 *
		 * Note that this container can only hold simple types that do not perform allocations. Therefore,
		 * there is probably no benefit in moving the new item in over copying it in with
		 * @ref insert(const_iterator, const Tp&).
		 * @param position Position of the new element. The new element will be added before the old element
		 * at that position.
		 * @param item Value of the new item.
		 * @return Position of the new element.
		 */
		iterator insert(const_iterator position, Tp&& item)
		{
			if (_end == _endOfStorage)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(1), index, 1);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + 1, position, (_end - position) * sizeof(Tp));
				++_end;
			}
			*const_cast<iterator>(position) = std::move(item);
			return const_cast<iterator>(position);
		}

		/** @brief Insert elements at a given position and initialize them from a initializer list.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive.
		 * @param position Position of the new elements. The new elements will be added before the old element
		 * at that position.
		 * @param initlist List of items to insert.
		 * @return Position of the first new element.
		 */
		iterator insert(const_iterator position, std::initializer_list<Tp> initlist)
		{
			if (capacity() < size() + initlist.size())
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(initlist.size()), index, initlist.size());
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + initlist.size(), position, (_end - position) * sizeof(Tp));
				_end += initlist.size();
			}
			iterator destIter = const_cast<iterator>(position);
			for (typename std::initializer_list<Tp>::const_iterator i = initlist.begin(); i != initlist.end(); ++i)
			{
				*destIter = *i;
				++destIter;
			}
			return const_cast<iterator>(position);
		}

		/** @brief Delete an element from the container.
		 * @details This operation moves all elements past the removed element, and can therefore be
		 * expensive.
		 * @param position Position of element to be removed.
		 * @return Iterator pointing to the first element past the delete element.
		 */
		iterator erase(const_iterator position)
		{
			--_end;
			memmove(const_cast<iterator>(position), position + 1, (_end - position) * sizeof(Tp));
			return const_cast<iterator>(position);
		}

		/** @brief Delete a range of elements from the container.
		 * @details This operation moves all elements past the removed elements, and can therefore be
		 * expensive.
		 * @param first Position of first element to be removed.
		 * @param last Position past last element to be removed.
		 * @return Iterator pointing to the first element past the delete element.
		 */
		iterator erase(const_iterator first, const_iterator last)
		{
			size_t n = last - first;
			_end -= n;
			memmove(const_cast<iterator>(first), first + n, (_end - first) * sizeof(Tp));
			return const_cast<iterator>(first);
		}

		/** @brief Swap the contents of this uvector with the given uvector.
		 * @details Iterators to both vectors will remain valid and will point into
		 * to the swapped container afterwards. This function will never reallocate
		 * space.
		 *
		 * The allocator will be swapped when the @c propagate_on_container_swap
		 * of the respective @c allocator_trait is @c true_type.
		 * Its behaviour is undefined when the allocators do not compare equal and
		 * @c propagate_on_container_swap is false.
		 * @param other Other uvector whose contents it to be swapped with this.
		 */
		void swap(uvector<Tp, Alloc>& other) noexcept
		{
			swap(other, typename std::allocator_traits<Alloc>::propagate_on_container_swap());
		}

		/** @brief Remove all elements from the container. */
		void clear()
		{
			_end = _begin;
		}

		/** @brief Insert an element at a given position by constructing it in place.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive.
		 * @param position Position of the new element. The new element will be added before the old element
		 * at that position.
		 * @param args List of arguments to be forwarded to construct the new element.
		 * @return Position of the new element.
		 */
		template<typename... Args>
		iterator emplace(const_iterator position, Args&&... args)
		{
			if (_end == _endOfStorage)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(1), index, 1);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + 1, position, (_end - position) * sizeof(Tp));
				++_end;
			}
			*const_cast<iterator>(position) = Tp(std::forward<Args>(args)...);
			return const_cast<iterator>(position);
		}

		/** @brief Add the given value to the end of the container by constructing it in place.
		 * @details Iterators are invalidated.
		 * @param args List of arguments to be forwarded to construct the new element.
		 */
		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			if (_end == _endOfStorage)
				enlarge(enlarge_size(1));
			*_end = Tp(std::forward<Args>(args)...);
			++_end;
		}

		/** @brief Get a copy of the allocator. */
		allocator_type get_allocator() const noexcept
		{
			return *this;
		}

		// --- NON STANDARD METHODS ---

		/** @brief Insert elements at a given position without initializing them.
		 * @details All iterators will be invalidated. This operation needs to move all elements after
		 * the new element, and can therefore be expensive. It will not initialize the new elements,
		 * and is therefore faster than @ref insert(const_iterator, size_t, const Tp&).
		 *
		 * This method is non-standard: it is not present in std::vector.
		 * @param position Position of the new elements. The new elements will be added before the old element
		 * at that position.
		 * @param n Number of elements to add.
		 */
		iterator insert_uninitialized(const_iterator position, size_t n)
		{
			if (capacity() < size() + n)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(n), index, n);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + n, position, (_end - position) * sizeof(Tp));
				_end += n;
			}
			return const_cast<iterator>(position);
		}

		/** @brief Add a range of items to the end of the container.
		 * @details All iterators will be invalidated.
		 *
		 * This method is non-standard: it is not present in std::vector.
		 * @param first Iterator to the beginning of the range.
		 * @param last Iterator past the end of the range.
		 */
		template <class InputIterator>
		void push_back(InputIterator first, InputIterator last)
		{
			push_back_range<InputIterator>(first, last, std::is_integral<InputIterator>());
		}

		/** @brief Add elements at the end and initialize them with a value.
		 * @details All iterators will be invalidated.
		 *
		 * This method is non-standard: it is not present in std::vector.
		 * @param n Number of elements to add.
		 * @param val Value of the new items.
		 */
		void push_back(size_t n, const Tp& val)
		{
			if (capacity() - size() < n)
			{
				enlarge(enlarge_size(n));
			}
			std::uninitialized_fill_n<Tp*, size_t>(_end, n, val);
			_end += n;
		}

		/** @brief Add elements from an initializer list to the end of the container.
		 * @details All iterators will be invalidated.
		 *
		 * This method is non-standard: it is not present in std::vector.
		 * @param initlist The list with values to add.
		 */
		void push_back(std::initializer_list<Tp> initlist)
		{
			if (capacity() - size() < initlist.size())
			{
				enlarge(enlarge_size(initlist.size()));
			}
			for (typename std::initializer_list<Tp>::iterator i = initlist.begin(); i != initlist.end(); ++i)
			{
				*_end = *i;
				++_end;
			}
		}

		/** @brief Add elements at the end without initializing them.
		 * @details All iterators will be invalidated.
		 *
		 * This method is non-standard: it is not present in std::vector.
		 * @param n Number of elements to add.
		 */
		void push_back_uninitialized(size_t n)
		{
			resize(size() + n);
		}

	private:

		pointer allocate(size_t n)
		{
			return Alloc::allocate(n);
		}

		void deallocate() noexcept
		{
			deallocate(_begin, capacity());
		}

		void deallocate(pointer begin, size_t n) noexcept
		{
			if (begin != nullptr)
				Alloc::deallocate(begin, n);
		}

		template<typename InputIterator>
		void construct_from_range(InputIterator first, InputIterator last, std::false_type)
		{
			construct_from_range<InputIterator>(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}

		template<typename Integral>
		void construct_from_range(Integral n, Integral val, std::true_type)
		{
			_begin = allocate(n);
			_end = _begin + n;
			_endOfStorage = _end;
			std::uninitialized_fill_n<Tp*, size_t>(_begin, n, val);
		}

		template<typename InputIterator>
		void construct_from_range(InputIterator first, InputIterator last, std::forward_iterator_tag)
		{
			size_t n = std::distance(first, last);
			_begin = allocate(n);
			_end = _begin + n;
			_endOfStorage = _begin + n;
			Tp* destIter = _begin;
			while (first != last)
			{
				*destIter = *first;
				++destIter; ++first;
			}
		}

		template<typename InputIterator>
		void assign_from_range(InputIterator first, InputIterator last, std::false_type)
		{
			assign_from_range<InputIterator>(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}

		// This function is called from assign(iter,iter) when Tp is an integral. In that case,
		// the user tried to call assign(n, &val), but it got caught by the wrong overload.
		template<typename Integral>
		void assign_from_range(Integral n, Integral val, std::true_type)
		{
			if (size_t(n) > capacity())
			{
				iterator newStorage = allocate(n);
				deallocate();
				_begin = newStorage;
				_endOfStorage = _begin + n;
			}
			_end = _begin + n;
			std::uninitialized_fill_n<Tp*, size_t>(_begin, n, val);
		}

		template<typename InputIterator>
		void assign_from_range(InputIterator first, InputIterator last, std::forward_iterator_tag)
		{
			size_t n = std::distance(first, last);
			if (n > capacity())
			{
				iterator newStorage = allocate(n);
				deallocate();
				_begin = newStorage;
				_endOfStorage = _begin + n;
			}
			_end = _begin + n;
			Tp* destIter = _begin;
			while (first != last)
			{
				*destIter = *first;
				++destIter; ++first;
			}
		}

		template<typename InputIterator>
		iterator insert_from_range(const_iterator position, InputIterator first, InputIterator last, std::false_type)
		{
			return insert_from_range<InputIterator>(position, first, last,
				typename std::iterator_traits<InputIterator>::iterator_category());
		}

		template<typename Integral>
		iterator insert_from_range(const_iterator position, Integral n, Integral val, std::true_type)
		{
			if (capacity() < size() + n)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(n), index, n);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + n, position, (_end - position) * sizeof(Tp));
				_end += n;
			}
			std::uninitialized_fill_n<Tp*, size_t>(const_cast<iterator>(position), n, val);
			return const_cast<iterator>(position);
		}

		template<typename InputIterator>
		iterator insert_from_range(const_iterator position, InputIterator first, InputIterator last, std::forward_iterator_tag)
		{
			size_t n = std::distance(first, last);
			if (capacity() < size() + n)
			{
				size_t index = position - _begin;
				enlarge_for_insert(enlarge_size(n), index, n);
				position = _begin + index;
			}
			else {
				memmove(const_cast<iterator>(position) + n, position, (_end - position) * sizeof(Tp));
				_end += n;
			}
			Tp* destIter = const_cast<iterator>(position);
			while (first != last)
			{
				*destIter = *first;
				++destIter; ++first;
			}
			return const_cast<iterator>(position);
		}

		void check_bounds(size_t index) const
		{
			if (index >= size())
				throw std::out_of_range("Access to element in uvector past end");
		}

		size_t enlarge_size(size_t extra_space_needed) const noexcept
		{
			return size() + std::max(size(), extra_space_needed);
		}

		void enlarge(size_t newSize)
		{
			pointer newStorage = allocate(newSize);
			memcpy(newStorage, _begin, size() * sizeof(Tp));
			deallocate();
			_end = newStorage + size();
			_begin = newStorage;
			_endOfStorage = _begin + newSize;
		}

		void enlarge_for_insert(size_t newSize, size_t insert_position, size_t insert_count)
		{
			pointer newStorage = allocate(newSize);
			memcpy(newStorage, _begin, insert_position * sizeof(Tp));
			memcpy(newStorage + insert_position + insert_count, _begin + insert_position, (size() - insert_position) * sizeof(Tp));
			deallocate();
			_end = newStorage + size() + insert_count;
			_begin = newStorage;
			_endOfStorage = _begin + newSize;
		}

		// implementation of operator=(const&) without propagate_on_container_copy_assignment
		uvector& assign_copy_from(const uvector<Tp, Alloc>& other, std::false_type)
		{
			const size_t n = other.size();
			if (n > capacity()) {
				iterator newStorage = allocate(n);
				deallocate();
				_begin = newStorage;
				_end = _begin + n;
				_endOfStorage = _end;
			}
			memcpy(_begin, other._begin, n * sizeof(Tp));
			return *this;
		}

		// implementation of operator=(const&) with propagate_on_container_copy_assignment
		uvector& assign_copy_from(const uvector<Tp, Alloc>& other, std::true_type)
		{
			if (allocator_is_always_equal() || static_cast<Alloc&>(other) == static_cast<Alloc&>(*this))
			{
				assign_copy_from(other, std::false_type());
			}
			else {
				const size_t n = other.size();
				iterator newStorage = static_cast<Alloc&>(other).allocate(n);
				deallocate();
				_begin = newStorage;
				_end = _begin + n;
				_endOfStorage = _end;
				memcpy(_begin, other._begin, n * sizeof(Tp));
				Alloc::operator=(static_cast<Alloc&>(other));
			}
			return *this;
		}

		// implementation of operator=() without propagate_on_container_move_assignment
		uvector& assign_move_from(uvector<Tp, Alloc>&& other, std::false_type) noexcept(allocator_is_always_equal::value)
		{
			if (allocator_is_always_equal::value || static_cast<Alloc&>(other) == static_cast<Alloc&>(*this))
			{
				deallocate();
				_begin = other._begin;
				_end = other._end;
				_endOfStorage = other._endOfStorage;
				other._begin = nullptr;
				other._end = nullptr;
				other._endOfStorage = nullptr;
			}
			else {
				// We should not propagate the allocator and the allocators are different.
				// This means we can not swap the allocated space, since then we would
				// deallocate the space with a different allocator type. Therefore, we
				// need to copy:
				assign_copy_from(other, std::false_type());
			}
			return *this;
		}

		// implementation of operator=() with propagate_on_container_move_assignment
		uvector& assign_move_from(uvector<Tp, Alloc>&& other, std::true_type) noexcept
		{
			deallocate();
			Alloc::operator=(std::move(static_cast<Alloc&>(other)));
			_begin = other._begin;
			_end = other._end;
			_endOfStorage = other._endOfStorage;
			other._begin = nullptr;
			other._end = nullptr;
			other._endOfStorage = nullptr;
			return *this;
		}

		// implementation of swap with propagate_on_container_swap
		void swap(uvector<Tp, Alloc>& other, std::true_type) noexcept
		{
			std::swap(_begin, other._begin);
			std::swap(_end, other._end);
			std::swap(_endOfStorage, other._endOfStorage);
			std::swap(static_cast<Alloc&>(other), static_cast<Alloc&>(*this));
		}

		// implementation of swap without propagate_on_container_swap
		void swap(uvector<Tp, Alloc>& other, std::false_type) noexcept
		{
			std::swap(_begin, other._begin);
			std::swap(_end, other._end);
			std::swap(_endOfStorage, other._endOfStorage);
			/**
			 * We have two choices here:
			 * - Do not swap the allocators. For stateful allocators, we would need to
			 *   reallocate memory, and iterators would not be valid UNLESS
			 *   they were stored as indices. However, containers with stateful allocators
			 *   are not allowed to be swapped unless the allocators are equal, in which case swapping
			 *   is not necessary.
			 * - Swap the allocators. This would not reallocate memory and
			 *   iterators remain valid, but the trait ignores propagate_on_container_swap.
			 *
			 * The standard says:
			 * "Allocator replacement is performed by copy assignment, move assignment, or
			 * swapping of the allocator only if allocator_traits<allocatortype>::
			 * propagate_on_container_copy_assignment::value,
			 * allocator_traits<allocatortype>::propagate_on_container_move_assignment::value,
			 * or allocator_traits<allocatortype>::propagate_on_container_swap::value is true
			 * within the implementation of the corresponding container operation. The behavior
			 * of a call to a container’s swap function is undefined unless the objects being
			 * swapped have allocators that compare equal or
			 * allocator_traits<allocatortype>::propagate_on_container_swap::value is true."
			 */
		}

		template<typename InputIterator>
		void push_back_range(InputIterator first, InputIterator last, std::false_type)
		{
			push_back_range<InputIterator>(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}

		// This function is called from push_back(iter,iter) when Tp is an integral. In that case,
		// the user tried to call push_back(n, &val), but it got caught by the wrong overload.
		template<typename Integral>
		void push_back_range(Integral n, Integral val, std::true_type)
		{
			if (capacity() - size() < size_t(n))
			{
				enlarge(enlarge_size(n));
			}
			std::uninitialized_fill_n<Tp*, size_t>(_end, n, val);
			_end += n;
		}

		template<typename InputIterator>
		void push_back_range(InputIterator first, InputIterator last, std::forward_iterator_tag)
		{
			size_t n = std::distance(first, last);
			if (n > capacity() - size())
			{
				enlarge(enlarge_size(n));
			}
			while (first != last)
			{
				*_end = *first;
				++_end;
				++first;
			}
		}

	};

	/** @brief Compare two uvectors for equality. */
	template<class Tp, class Alloc>
	inline bool operator==(const uvector<Tp, Alloc>& lhs, const uvector<Tp, Alloc>& rhs) noexcept
	{
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	/** @brief Compare two uvectors for inequality. */
	template<class Tp, class Alloc>
	inline bool operator!=(const uvector<Tp, Alloc>& lhs, const uvector<Tp, Alloc>& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	/** @brief Compare two uvectors for smaller than.
	 * @details If two uvectors compare equal up to the length of one, the uvector with
	 * the smallest size is consider to be smaller.
	 */
	template <class Tp, class Alloc>
	inline bool operator<(const uvector<Tp, Alloc>& lhs, const uvector<Tp, Alloc>& rhs) noexcept
	{
		const size_t minSize = std::min(lhs.size(), rhs.size());
		for (size_t i = 0; i != minSize; ++i)
		{
			if (lhs[i] < rhs[i])
				return true;
			else if (lhs[i] > rhs[i])
				return false;
		}
		return lhs.size() < rhs.size();
	}

	/** @brief Compare two uvectors for smaller than or equal.
	 * @details If two uvectors compare equal up to the length of one, the uvector with
	 * the smallest size is consider to be smaller.
	 */
	template <class Tp, class Alloc>
	inline bool operator<=(const uvector<Tp, Alloc>& lhs, const uvector<Tp, Alloc>& rhs) noexcept
	{
		const size_t minSize = std::min(lhs.size(), rhs.size());
		for (size_t i = 0; i != minSize; ++i)
		{
			if (lhs[i] < rhs[i])
				return true;
			else if (lhs[i] > rhs[i])
				return false;
		}
		return lhs.size() <= rhs.size();
	}

	/** @brief Compare two uvectors for larger than.
	 * @details If two uvectors compare equal up to the length of one, the uvector with
	 * the smallest size is consider to be smaller.
	 */
	template <class Tp, class Alloc>
	inline bool operator>(const uvector<Tp, Alloc>& lhs, const uvector<Tp, Alloc>& rhs) noexcept
	{
		return rhs < lhs;
	}

	/** @brief Compare two uvectors for larger than or equal.
	 * @details If two uvectors compare equal up to the length of one, the uvector with
	 * the smallest size is consider to be smaller.
	 */
	template <class Tp, class Alloc>
	inline bool operator>=(const uvector<Tp, Alloc>& lhs, const uvector<Tp, Alloc>& rhs) noexcept
	{
		return rhs <= lhs;
	}

	/** @brief Swap the contents of the two uvectors.
		* @details Iterators to both vectors will remain valid and will point into
		* to the swapped container afterwards. This function will never reallocate
		* space.
		*
		* The allocator will be swapped when the @c propagate_on_container_swap
		* of the respective @c allocator_trait is @c true_type.
		* Its behaviour is undefined when the allocators do not compare equal and
		* @c propagate_on_container_swap is false.
		*/
	template <class Tp, class Alloc>
	inline void swap(uvector<Tp, Alloc>& x, uvector<Tp, Alloc>& y)
	{
		x.swap(y);
	}
	/** @} */

} // end of namespace ao
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

//ToDo: Get rid of once waitable task system is implemented for lock based approach.
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
			invoke_f = nullptr;
			destroy_f = nullptr;
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
	ao::uvector<move_only_function_32<void()>> taskQueue;
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
			auto size = queue.size();
			queue.resize(size + 1);
			new (&queue[size]) move_only_function_32<void()>(std::move(defferedWrite));
			//queue.emplace_back(std::move(defferedWrite));
		}
	}

	//Fast push to the write task queue. Write to the same object can be stacked and will be logically cohorent
	//Sig: Accepts void()
	template<typename memfn>
	void stackingWrite(memfn&& func)
	{
		auto& queue = (*ptr.poolHeadPtr)[ptr.clusterId].taskQueue;
		auto defferedWrite = [func = std::forward<memfn>(func), this]() { std::invoke(func, ptr.get()); };
		auto size = queue.size();
		queue.resize(size + 1);
		new (&queue[size]) move_only_function_32<void()>(std::move(defferedWrite));
		//queue.emplace_back(std::move(defferedWrite));
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
			auto vec2 = ao::uvector<move_only_function_32<void()>>();
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
				auto vec2 = ao::uvector<move_only_function_32<void()>>();
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
