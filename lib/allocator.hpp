#ifndef gintonic_allocator_hpp
#define gintonic_allocator_hpp

namespace gintonic {

/*****************************************************************************
 * gintonic::allocator                                                       *
 *                                                                           *
 * Purpose: To be able to put SIMD types in containers, as well as classes   *
 * that contain SIMD types (e.g. vec3f et al)                                *
 ****************************************************************************/
template <class T, std::size_t Alignment = 16> class allocator
{
public:
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef std::size_t size_type;
	typedef ptrdiff_t difference_type;

	inline T* address(T& t) const
	{
		return std::addressof(t);
	}

	inline const T * address(const T& t) const
	{
		return std::addressof(t);
	}

	inline std::size_t max_size() const
	{
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<std::size_t>(0) - static_cast<std::size_t>(1)) 
			/ sizeof(T);
	}

	// What follows is always the same for all allocators.
	template <typename U> struct rebind
	{
		typedef allocator<U, Alignment> other;
	};

	inline bool operator!=(const allocator& other) const
	{
		return !(*this == other);
	}

	inline void construct(T* const p, const T& t) const
	{
		new (p) T(t);
		// void* const pv = static_cast<void*>(p);
		// new (pv) T(t);
	}

	inline void destroy(T * const p) const
	{
		p->~T();
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	inline bool operator==(const allocator& other) const
	{
		return true;
	}


	// Default constructor, copy constructor, rebinding constructor, and
	// destructor. Empty for stateless allocators.
	inline allocator() = default;

	inline allocator(const allocator&) = default;

	template <typename U> inline allocator(const allocator<U, Alignment>&) {}

	inline ~allocator() {}


	// The following will be different for each allocator.
	inline T* allocate(const std::size_t n) const
	{
		// The return value of allocate(0) is unspecified.
		// Mallocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		// if (n == 0) 
		// {
		// 	return nullptr;
		// }

		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		// if (n > max_size())
		// {
		// 	throw std::length_error(
		// 		"gintonic::allocator<T>::allocate() - Integer overflow.");
		// }

		// Mallocator wraps malloc().
		// void* const pv = _mm_malloc(n * sizeof(T), Alignment);

		// Allocators should throw std::bad_alloc in the case of memory 
		// allocation failure.
		// if (pv == nullptr)
		// {
		// 	throw std::bad_alloc();
		// }

		// return static_cast<T*>(pv);

		return static_cast<T*>(_mm_malloc(n * sizeof(T), Alignment));
	}

	inline void deallocate(T* const p, const std::size_t n) const
	{
		_mm_free(p);
	}


	// The following will be the same for all allocators that ignore hints.
	template <typename U>
	inline T* allocate(const std::size_t n, const U* /* const hint */) const
	{
		return allocate(n);
	}

	allocator& operator=(const allocator&) = delete;
};

} // namespace gintonic

#endif