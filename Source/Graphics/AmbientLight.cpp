#include "AmbientLight.hpp"

#include "Renderer.hpp"
#include "shaders.hpp"
#include "Mesh.hpp"

#include <iostream>

namespace gintonic {

AmbientLight::AmbientLight(const vec4f& intensity)
: Light(intensity)
{
	/* Empty on purpose. */
}

void AmbientLight::shine(const Entity& e) const noexcept
{
	const auto& s = Renderer::get_lp_ambient_shader();
	s.activate();
	s.set_viewport_size(Renderer::viewportSize());
	s.set_light_intensity(this->mIntensity);
	Renderer::getUnitQuad()->draw();
}

std::ostream& operator << (std::ostream& os, const AmbientLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& AmbientLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (AmbientLight) intensity: " << this->mIntensity << " }";
}

} // namespace gintonic
