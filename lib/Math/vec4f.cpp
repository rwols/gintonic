#include "Math/vec4f.hpp"
#include "Math/vec3f.hpp"
#include "Math/vec2f.hpp"
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

} // namespace gintonic
