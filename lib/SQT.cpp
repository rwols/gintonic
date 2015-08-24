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