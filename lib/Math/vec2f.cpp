#include "Math/vec2f.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk/core/math/fbxvector2.h>
#include <array>

namespace gintonic {

vec2f::vec2f(std::initializer_list<float> init) noexcept
{
	GT_PROFILE_FUNCTION;

	GINTONIC_ALIGNED(16) float temp[2];
	std::copy(init.begin(), init.end(), temp);
	x = temp[0];
	y = temp[1];
}

vec2f::vec2f(const FBXSDK_NAMESPACE::FbxVector2& v) noexcept
: x(static_cast<float>(v[0]))
, y(static_cast<float>(v[1]))
{
	GT_PROFILE_FUNCTION;
}

vec2f& vec2f::operator=(std::initializer_list<float> init) noexcept
{
	GT_PROFILE_FUNCTION;

	GINTONIC_ALIGNED(16) float temp[2];
	std::copy(init.begin(), init.end(), temp);
	x = temp[0];
	y = temp[1];
	return *this;
}

vec2f& vec2f::operator=(const FBXSDK_NAMESPACE::FbxVector2& v) noexcept
{
	GT_PROFILE_FUNCTION;

	x = static_cast<float>(v[0]);
	y = static_cast<float>(v[1]);
	return *this;
}

vec2f::operator FBXSDK_NAMESPACE::FbxVector2() const noexcept
{
	GT_PROFILE_FUNCTION;
	
	return FBXSDK_NAMESPACE::FbxVector2(x, y);
}

} // namespace gintonic