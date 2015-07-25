#include "math.hpp"

namespace gintonic {

std::ostream& operator << (std::ostream& os, const FbxVector2& v)
{
	return os << '[' << v[0] << ", " << v[1] << ']';
}

std::ostream& operator << (std::ostream& os, const FbxVector4& v)
{
	return os << '[' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ']';
}

std::ostream& operator << (std::ostream& os, const FbxColor& c)
{
	return os << '[' << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << ']';
}

} // end of namespace gintonic