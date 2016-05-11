#include "vec4f.hpp"
#include "vec3f.hpp"
#include "vec2f.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector4.h>
#include <array>

namespace gintonic {

vec4f::vec4f(std::initializer_list<float> init) noexcept
{
	GT_PROFILE_FUNCTION;

	GINTONIC_ALIGNED(16) float temp[4];
	std::copy(init.begin(), init.end(), temp);
	x = temp[0];
	y = temp[1];
	z = temp[2];
	w = temp[3];
}

vec4f::vec4f(const vec2f& v, const float z, const float w) noexcept
: x(v.x), y(v.y), z(z), w(w)
{
	GT_PROFILE_FUNCTION;
}

vec4f::vec4f(const vec2f& a, const vec2f& b) noexcept
: x(a.x), y(a.y), z(b.x), w(b.y)
{
	GT_PROFILE_FUNCTION;
}

vec4f::vec4f(const vec3f& v, const float w) noexcept
: x(v.x), y(v.y), z(v.z), w(w)
{
	GT_PROFILE_FUNCTION;
}

vec4f::vec4f(const FBXSDK_NAMESPACE::FbxVector4& v) noexcept
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
, z(static_cast<float>(v[2]))
, w(static_cast<float>(v[3]))
{
	GT_PROFILE_FUNCTION;
}

vec4f& vec4f::operator=(std::initializer_list<float> init) noexcept
{
	GT_PROFILE_FUNCTION;

	GINTONIC_ALIGNED(16) float temp[4];
	std::copy(init.begin(), init.end(), temp);
	x = temp[0];
	y = temp[1];
	z = temp[2];
	w = temp[3];
	return *this;
}

vec4f& vec4f::operator=(const FBXSDK_NAMESPACE::FbxVector4& v) noexcept
{
	GT_PROFILE_FUNCTION;

	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	z = static_cast<float>(v[2]);
	w = static_cast<float>(v[3]);
	return *this;
}

vec4f::operator FBXSDK_NAMESPACE::FbxVector4() const noexcept
{
	GT_PROFILE_FUNCTION;
	
	return FBXSDK_NAMESPACE::FbxVector4(x, y, z, w);
}

} // namespace gintonic