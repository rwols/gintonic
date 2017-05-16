#include "Graphics/Light.hpp"

#include "Foundation/exception.hpp"

#include "Graphics/AmbientLight.hpp"
#include "Graphics/DirectionalLight.hpp"
#include "Graphics/PointLight.hpp"
#include "Graphics/SpotLight.hpp"

#include <iostream>

#include <fbxsdk.h>

namespace gintonic
{

Light::Light(const vec4f& intensity) : mIntensity(intensity)
{
    /* Empty on purpose. */
}

void Light::setBrightness(const float brightness) { mIntensity.w = brightness; }

float Light::getBrightness() const noexcept { return mIntensity.w; }

std::ostream& operator<<(std::ostream& os, const Light* l)
{
    return l->prettyPrint(os);
}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<Light>& l)
{
    return l->prettyPrint(os);
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Light>& l)
{
    return l->prettyPrint(os);
}

std::ostream& operator<<(std::ostream& os, const Light& l)
{
    return l.prettyPrint(os);
}

std::ostream& Light::prettyPrint(std::ostream& os) const noexcept
{
    return os << "{ (Light) intensity: " << mIntensity << " }";
}

} // namespace gintonic
