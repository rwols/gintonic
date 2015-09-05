/**
 * @file allocator.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_allocator_hpp
#define gintonic_allocator_hpp

namespace gintonic {

/**
 * @brief Aligned allocator.
 * @details The aligned allocator can be used with all STL
 * containers so as to make sure that types are aligned on a
 * given memory boundary.
 * 
 * @tparam T The type to allocate.
 * @tparam Alignment The memory alignment. By default, this is
 * set to 16.
 */
template <class T, std::size_t Alignment = 16> class allocator
{
public:

	/// The pointer type.
	typedef T* pointer;

	/// The constant pointer type.
	typedef const T* const_pointer;

	/// The reference type.
	typedef T& reference;

	/// The constant reference type.
	typedef const T& const_reference;

	/// The value type.
	typedef T value_type;

	/// The size type.
	typedef std::size_t size_type;

	/// The difference type.
	typedef ptrdiff_t difference_type;

	/**
	 * @brief Get the memory address.
	 * @param t The reference to get the memory address from.
	 * @return A pointer to the object.
	 */
	inline T* address(T& t) const
	{
		return std::addressof(t);
	}

	/**
	 * @brief Get the memory address.
	 * @param t The reference to get the memory address from.
	 * @return A pointer to the object.
	 */
	inline const T * address(const T& t) const
	{
		return std::addressof(t);
	}

	/**
	 * @brief The maximum allocation size.
	 * @return The maximum allocation size.
	 */
	inline std::size_t max_size() const
	{
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<std::size_t>(0) - static_cast<std::size_t>(1)) 
			/ sizeof(T);
	}

	/// Rebind an allocator. Does nothing interesting.
	template <typename U> struct rebind
	{
		typedef allocator<U, Alignment> other;
	};

	/// Always returns false.
	inline bool operator!=(const allocator& other) const
	{
		return !(*this == other);
	}

	/**
	 * @brief Copy construct an object.
	 * @param p The location where to construct.
	 * @param t The object to copy.
	 */
	inline void construct(T* const p, const T& t) const
	{
		new (p) T(t);
	}

	/**
	 * @brief Explicitly call the destructor of an object.
	 * @param p A constant pointer to destroy.
	 */
	inline void destroy(T * const p) const
	{
		p->~T();
	}

	/// Always returns true.
	inline bool operator==(const allocator& other) const
	{
		return true;
	}

	/// Default constructor.
	inline allocator() = default;

	/// Default copy constructor.
	inline allocator(const allocator&) = default;

	/// Copy constructor from another template type U, but with the same alignment.
	template <typename U> inline allocator(const allocator<U, Alignment>&) {}

	/// Default destructor does nothing interesting.
	inline ~allocator() {}

	/**
	 * @brief Allocate a number of objects.
	 * @param n The number of objects of type T to allocate.
	 * @return A pointer to the first allocated object.
	 */
	inline T* allocate(const std::size_t n) const
	{
		return static_cast<T*>(_mm_malloc(n * sizeof(T), Alignment));
	}

	/**
	 * @brief Deallocate a number of objects.
	 * @param p A pointer to the first object.
	 * @param n The number of objects to deallocate.
	 */
	inline void deallocate(T* const p, const std::size_t n) const
	{
		_mm_free(p);
	}

	/**
	 * @brief Allocate a number of objects with a hint. The hint is ignored.
	 * @param n The number of objects to allocate.
	 * @return A pointer to the first allocated object.
	 */
	template <typename U>
	inline T* allocate(const std::size_t n, const U* /* const hint */) const
	{
		return allocate(n);
	}

	/// You cannot copy an allocator.
	allocator& operator=(const allocator&) = delete;
};

} // namespace gintonic

#endif