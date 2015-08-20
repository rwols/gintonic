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

	inline SQT() = default;

	inline SQT(const vec3f& scale, const quatf& rotation, const vec3f& translation)
	: scale(scale)
	, rotation(rotation)
	, translation(translation)
	{
		/* Empty on purpose. */
	}

	inline SQT operator % (const SQT& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return SQT(scale * other.scale, rotation * other.rotation, translation + other.translation);
	}

	SQT& operator %= (const SQT& other) BOOST_NOEXCEPT_OR_NOTHROW
	{
		scale *= other.scale;
		rotation *= other.rotation;
		translation += other.translation;
		return *this;
	}

	inline void look_at(const SQT& other) BOOST_NOEXCEPT_OR_NOTHROW
	{
		rotation = quatf::look_at(translation, other.translation, vec3f(0.0f, 1.0f, 0.0f));
	}

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

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

} // namespace gintonic

#endif