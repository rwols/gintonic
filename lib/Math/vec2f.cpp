#include "Math/vec2f.hpp"
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

vec2f& vec2f::operator=(std::initializer_list<float> init) noexcept
{
    GT_PROFILE_FUNCTION;

    GINTONIC_ALIGNED(16) float temp[2];
    std::copy(init.begin(), init.end(), temp);
    x = temp[0];
    y = temp[1];
    return *this;
}

} // namespace gintonic
