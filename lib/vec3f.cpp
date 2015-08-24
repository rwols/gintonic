#include "vec3f.hpp"
#include "vec2f.hpp"
#include "quatf.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector4.h>
#include <array>

namespace gintonic {

vec3f::vec3f(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW
{
	GINTONIC_ALIGNED(16) float temp[3];
	std::copy(init.begin(), init.end(), temp);
	x = temp[0];
	y = temp[1];
	z = temp[2];
	dummy = 0.0f;
}

vec3f::vec3f(const vec2f& v, const float z) BOOST_NOEXCEPT_OR_NOTHROW
: x(v.x), y(v.y), z(z), dummy(0.0f)
{
	/* Empty on purpose. */
}

vec3f::vec3f(const quatf& q) BOOST_NOEXCEPT_OR_NOTHROW
: x(q.x), y(q.y), z(q.z), dummy(0.0f)
{
	/* Empty on purpose. */
}

vec3f::vec3f(const FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
, z(static_cast<float>(v[2]))
, dummy(0.0f)
{
	/* Empty on purpose. */
}

vec3f& vec3f::operator=(std::initializer_list<float> init) BOOST_NOEXCEPT_OR_NOTHROW
{
	GINTONIC_ALIGNED(16) float temp[3];
	std::copy(init.begin(), init.end(), temp);
	x = temp[0];
	y = temp[1];
	z = temp[2];
	dummy = 0.0f;
	return *this;
}

vec3f& vec3f::operator=(const FBX::FbxVector4& v) BOOST_NOEXCEPT_OR_NOTHROW
{
	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	z = static_cast<float>(v[2]);
	dummy = 0.0f;
	return *this;
}

vec3f::operator FBX::FbxVector4() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return FBX::FbxVector4(x, y, z, 0.0);
}

} // namespace gintonic