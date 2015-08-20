#include "vec4f.hpp"
#include "vec3f.hpp"
#include "vec2f.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector4.h>


namespace gintonic {

vec4f::vec4f(const vec2f& v, const float z, const float w) BOOST_NOEXCEPT_OR_NOTHROW
: x(v.x), y(v.y), z(z), w(w)
{
	/* Empty on purpose. */
}

vec4f::vec4f(const vec2f& a, const vec2f& b) BOOST_NOEXCEPT_OR_NOTHROW
: x(a.x), y(a.y), z(b.x), w(b.y)
{
	/* Empty on purpose. */
}

vec4f::vec4f(const vec3f& v, const float w) BOOST_NOEXCEPT_OR_NOTHROW
: x(v.x), y(v.y), z(v.z), w(w)
{
	/* Empty on purpose. */
}

vec4f::vec4f(const GINTONIC_NAMESPACE_FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
, z(static_cast<float>(v[2]))
, w(static_cast<float>(v[3]))
{
	/* Empty on purpose. */
}

vec4f& vec4f::operator=(const GINTONIC_NAMESPACE_FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	z = static_cast<float>(v[2]);
	w = static_cast<float>(v[3]);
	return *this;
}

vec4f::operator GINTONIC_NAMESPACE_FBX::FbxVector4() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return GINTONIC_NAMESPACE_FBX::FbxVector4(x, y, z, w);
}

} // namespace gintonic