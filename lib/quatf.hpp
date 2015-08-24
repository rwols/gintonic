#ifndef gintonic_quatf_hpp
#define gintonic_quatf_hpp

#include "utilities.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

union vec2f; // Forward declaration.
union vec3f; // Forward declaration.

/*****************************************************************************
* gintonic::quatf                                                            *
*****************************************************************************/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif

union quatf
{
private:

	__m128 data;

public:

	struct
	{
		float x;
		float y;
		float z;
		float w;
	};

	inline quatf() = default;

	inline quatf(const __m128& data) : data(data)
	{
		/* Empty on purpose. */
	}

	inline quatf(const float w, const float x, const float y, const float z)
	: x(x), y(y), z(z), w(w)  
	{
		/* Empty on purpose. */
	}

	quatf(const float realpart, const vec3f& imaginarypart);

	quatf operator * (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW;

	inline quatf& operator *= (const quatf& q) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return *this = *this * q;
	}

	inline quatf conjugate() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return quatf(w, -x, -y, -z);
	}

	vec3f apply_to(const vec3f& v) const BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f forward_direction() const BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f right_direction() const BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f up_direction() const BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f direction() const BOOST_NOEXCEPT_OR_NOTHROW;

	quatf& add_mousedelta(const vec2f&);

	float length2() const BOOST_NOEXCEPT_OR_NOTHROW;

	inline float length() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return std::sqrt(length2());
	}

	inline bool operator == (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	inline bool operator != (const quatf& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	static quatf axis_angle(
		const vec3f& rotation_axis, 
		const float rotation_angle);

	static quatf look_at(
		const vec3f& eye_position, 
		const vec3f& subject_position, 
		const vec3f& up_direction);

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(x) 
			& BOOST_SERIALIZATION_NVP(y) 
			& BOOST_SERIALIZATION_NVP(z)
			& BOOST_SERIALIZATION_NVP(w);
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline std::ostream& operator<<(std::ostream& os, const quatf& q)
{
	return os << q.w << ' ' << q.x << ' ' << q.y << ' ' << q.z;
}

inline std::istream& operator>>(std::istream& is, quatf& q)
{
	is >> q.w >> q.x >> q.y >> q.z;
	return is;
}

} // namespace gintonic

#endif