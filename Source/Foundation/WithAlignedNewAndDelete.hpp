#pragma once

#include "utilities.hpp"

namespace gintonic {

template <std::size_t Alignment>
struct WithAlignedNewAndDelete
{
public:
	inline static void* operator new(const std::size_t count)
	{
		return _mm_malloc(count, Alignment);
	}
	inline static void* operator new[](const std::size_t count)
	{
		return _mm_malloc(count, Alignment);
	}
	inline static void* operator new(const std::size_t /*count*/, void* here)
	{
		assert(isAligned(here, Alignment));
		return here;
	}
	inline static void* operator new[](const std::size_t /*count*/, void* here)
	{
		assert(isAligned(here, Alignment));
		return here;
	}
	inline static void operator delete(void* ptr)
	{
		_mm_free(ptr);
	}
	inline static void operator delete[](void* ptr)
	{
		_mm_free(ptr);
	}
	inline static void operator delete(void const*const ptr, void const*const here)
	{
		assert(isAligned(ptr, Alignment));
		assert(isAligned(here, Alignment));
	}
	inline static void oeprator delete[](void const*const ptr, void const*const here)
	{
		assert(isAligned(ptr, Alignment));
		assert(isAligned(here, Alignment));
	}
	inline static void operator delete(void const*const ptr, const std::size_t /*size*/)
	{
		assert(isAligned(ptr, Alignment));
	}
	inline static void operator delete[](void const*const ptr, const std::size_t /*size*/)
	{
		assert(isAligned(ptr, Alignment));
	}
};


} // namespace gintonic