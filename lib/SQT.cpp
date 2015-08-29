#include "SQT.hpp"

namespace gintonic {

SQT SQT::operator % (const SQT& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return SQT(
		scale * other.scale, 
		rotation * other.rotation, 
		translation + other.translation);
}

SQT& SQT::operator %= (const SQT& other) BOOST_NOEXCEPT_OR_NOTHROW
{
	scale *= other.scale;
	rotation *= other.rotation;
	translation += other.translation;
	return *this;
}

SQT SQT::inverse() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return SQT(
		vec3f(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z),
		rotation.conjugate(),
		-translation);
}

SQT& SQT::invert() BOOST_NOEXCEPT_OR_NOTHROW
{
	scale.x = 1.0f / scale.x;
	scale.y = 1.0f / scale.y;
	scale.z = 1.0f / scale.z;
	rotation = rotation.conjugate();
	translation *= -1.0f;
	return *this;
}

std::ostream& operator << (std::ostream& os, const SQT& sqt)
{
	return os << sqt.scale << ' ' << sqt.rotation << ' ' << sqt.translation;
}

std::istream& operator >> (std::istream& is, SQT& sqt)
{
	is >> sqt.scale >> sqt.rotation >> sqt.translation;
	return is;
}

} // namespace gintonic