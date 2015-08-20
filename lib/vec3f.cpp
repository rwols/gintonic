#include "vec3f.hpp"
#include "vec2f.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector4.h>

namespace gintonic {

vec3f::vec3f(const vec2f& v, const float z) BOOST_NOEXCEPT_OR_NOTHROW
: x(v.x), y(v.y), z(z)
{
	/* Empty on purpose. */
}

vec3f::vec3f(const GINTONIC_NAMESPACE_FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
, z(static_cast<float>(v[2]))
{
	/* Empty on purpose. */
}

vec3f& vec3f::operator=(const GINTONIC_NAMESPACE_FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	z = static_cast<float>(v[2]);
	return *this;
}

vec3f::operator GINTONIC_NAMESPACE_FBX::FbxVector4() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return GINTONIC_NAMESPACE_FBX::FbxVector4(x, y, z, 0.0);
}

} // namespace gintonic