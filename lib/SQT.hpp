#ifndef gintonic_SQT_hpp
#define gintonic_SQT_hpp

#include "vec3f.hpp"
#include "quatf.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

struct SQT
{
	vec3f scale;
	quatf rotation;
	vec3f translation;

	inline SQT()
	: scale(1.0f, 1.0f, 1.0f)
	, rotation(1.0f, 0.0f, 0.0f, 0.0f)
	, translation(0.0f, 0.0f, 0.0f)
	{
		/* Empty on purpose. */
	}

	inline SQT(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	: scale(scale)
	, rotation(rotation)
	, translation(translation)
	{
		/* Empty on purpose. */
	}

	SQT operator % (const SQT& other) const BOOST_NOEXCEPT_OR_NOTHROW;

	SQT& operator %= (const SQT& other) BOOST_NOEXCEPT_OR_NOTHROW;

	SQT inverse() const BOOST_NOEXCEPT_OR_NOTHROW;

	SQT& invert() BOOST_NOEXCEPT_OR_NOTHROW;

	inline void look_at(const SQT& other) BOOST_NOEXCEPT_OR_NOTHROW
	{
		rotation = quatf::look_at(translation, other.translation, vec3f(0.0f, 1.0f, 0.0f));
	}

	inline void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		translation += amount * rotation.forward_direction();
	}

	inline void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		translation -= amount * rotation.forward_direction();
	}

	inline void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		translation += amount * rotation.right_direction();
	}

	inline void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		translation -= amount * rotation.right_direction();
	}

	inline void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		translation += amount * rotation.up_direction();
	}

	inline void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW
	{
		translation -= amount * rotation.up_direction();
	}

	inline void add_mousedelta(const vec2f& delta) BOOST_NOEXCEPT_OR_NOTHROW
	{
		rotation.add_mousedelta(delta);
	}

	inline vec3f apply_to_point(const vec3f& point) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return rotation.apply_to(point) * scale + translation;
	}

	inline vec3f apply_to_direction(const vec3f& direction) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return rotation.apply_to(direction);
	}

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(scale) 
			& BOOST_SERIALIZATION_NVP(rotation) 
			& BOOST_SERIALIZATION_NVP(translation);
	}
};

inline float distance2(const SQT& a, const SQT& b) BOOST_NOEXCEPT_OR_NOTHROW
{
	return distance2(a.translation, b.translation);
}

inline float distance(const SQT& a, const SQT& b) BOOST_NOEXCEPT_OR_NOTHROW
{
	return distance(a.translation, b.translation);
}

std::ostream& operator << (std::ostream&, const SQT&);
std::istream& operator >> (std::istream&, SQT&);

} // namespace gintonic

#endif