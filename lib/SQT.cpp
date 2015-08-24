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

} // namespace gintonic