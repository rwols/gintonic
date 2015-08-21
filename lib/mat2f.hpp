#ifndef gintonic_mat2f_hpp
#define gintonic_mat2f_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

union vec2f; // Forward declaration.

/*****************************************************************************
* gintonic::mat2f, column-major storage                                      *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union mat2f
{
	__m128 data;

	struct
	{
		float m00;
		float m10;
		float m01;
		float m11;
	};

	inline mat2f() = default;

	inline mat2f(const float m00, const float m01, const float m10, const float m11)
	: m00(m00), m10(m10), m01(m01), m11(m11)
	{
		/* Empty on purpose. */
	}
	
	inline mat2f(const mat2f& m) : data(m.data)
	{
		/* Empty on purpose. */
	}
	
	inline mat2f(mat2f&& m) : data(m.data)
	{
		/* Empty on purpose. */
	}
	
	inline mat2f(__m128 data) : data(data)
	{
		/* Empty on purpose. */
	}
	
	mat2f(const vec2f& column0, const vec2f& column1);

	mat2f(std::initializer_list<float> init);
	
	inline mat2f& operator=(const mat2f& m)
	{
		data = m.data;
		return *this;
	}

	inline mat2f& operator=(mat2f&& m)
	{
		data = m.data;
		return *this;
	}

	mat2f& operator=(std::initializer_list<float> init);
	
	inline mat2f& operator += (const mat2f& v)
	{
		data = _mm_add_ps(data, v.data);
		return *this;
	}
	inline mat2f& operator -= (const mat2f& v)
	{
		data = _mm_sub_ps(data, v.data);
		return *this;
	}
	inline mat2f& operator *= (const mat2f& v)
	{
		data = _mm_hadd_ps(
			_mm_mul_ps(
				data, 
				_mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(3,1,2,0))), 
			_mm_mul_ps(
				data, 
				_mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(2,0,3,1))));
		data = _mm_shuffle_ps(data, data, _MM_SHUFFLE(1,3,2,0));
		return *this;
	}
	inline mat2f& operator *= (const float s)
	{
		data = _mm_mul_ps(data, _mm_load1_ps(&s));
		return *this;
	}
	
	inline mat2f& operator /= (const float s)
	{
		data = _mm_div_ps(data, _mm_load1_ps(&s));
		return *this;
	}
	
	inline bool operator == (const mat2f& m)
	{
		return m00 == m.m00 && m10 == m.m10 && m01 == m.m01 && m.m11;
	}
	
	inline bool operator != (const mat2f& m)
	{
		return !operator==(m);
	}

private:

	friend class boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & m00 & m10 & m01 & m11;
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline mat2f operator + (const mat2f& lhs, const mat2f& rhs)
{
	return _mm_add_ps(lhs.data, rhs.data);
}
inline mat2f operator - (const mat2f& lhs, const mat2f& rhs)
{
	return _mm_sub_ps(lhs.data, rhs.data);
}
vec2f operator * (const mat2f& m, const vec2f& v);
inline std::ostream& operator << (std::ostream& os, const mat2f& m)
{
	return os << m.m00 << ' ' << m.m10 << ' ' << m.m01 << ' ' << m.m11;
}
inline std::istream& operator >> (std::istream& is, mat2f& m)
{
	is >> m.m00 >> m.m10 >> m.m01 >> m.m11;
	return is;
}

} // namespace gintonic

#endif