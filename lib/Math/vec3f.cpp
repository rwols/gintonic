#include "Math/vec3f.hpp"
#include "Math/quatf.hpp"
#include "Math/vec2f.hpp"
#include <array>

namespace gintonic {

vec3f::vec3f(std::initializer_list<float> init) noexcept
{
    GT_PROFILE_FUNCTION;

    GINTONIC_ALIGNED(16) float temp[3];
    std::copy(init.begin(), init.end(), temp);
    x = temp[0];
    y = temp[1];
    z = temp[2];
    dummy = 0.0f;
}

vec3f::vec3f(const vec2f& v, const float z) noexcept
    : x(v.x), y(v.y), z(z), dummy(0.0f)
{
    GT_PROFILE_FUNCTION;
}

vec3f::vec3f(const quatf& q) noexcept : x(q.x), y(q.y), z(q.z), dummy(0.0f)
{
    GT_PROFILE_FUNCTION;
}

vec3f& vec3f::operator=(std::initializer_list<float> init) noexcept
{
    GT_PROFILE_FUNCTION;

    GINTONIC_ALIGNED(16) float temp[3];
    std::copy(init.begin(), init.end(), temp);
    x = temp[0];
    y = temp[1];
    z = temp[2];
    dummy = 0.0f;
    return *this;
}

} // namespace gintonic
