#include <gintonic/Graphics/AmbientLight.hpp>
#include <gintonic/Graphics/Mesh.hpp>
#include <gintonic/Graphics/Renderer.hpp>
#include <gintonic/Graphics/ShaderPrograms.hpp>
#include <iostream>

using namespace gintonic;

AmbientLight::AmbientLight(const vec4f& intensity) : Light(intensity)
{
    /* Empty on purpose. */
}

void AmbientLight::shine(
    const Entity& lightEntity,
    const std::vector<
        std::shared_ptr<Entity>>& /*shadowCastingGeometryEntities*/) const
    noexcept
{
    const auto& lProgram = AmbientLightShaderProgram::get();
    lProgram.activate();
    lProgram.setViewportSize(Renderer::viewportSize());
    lProgram.setLightIntensity(this->mIntensity);
    Renderer::getUnitQuad()->draw();
}

std::ostream& operator<<(std::ostream& os, const AmbientLight& l)
{
    return l.prettyPrint(os);
}

std::ostream& AmbientLight::prettyPrint(std::ostream& os) const noexcept
{
    return os << "{ (AmbientLight) intensity: " << this->mIntensity << " }";
}
