#include "AmbientLight.hpp"

#include "renderer.hpp"
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
	const auto& s = renderer::get_lp_ambient_shader();
	s.activate();
	s.set_viewport_size(renderer::viewport_size());
	s.set_light_intensity(intensity);
	renderer::getUnitQuad()->draw();
}

std::ostream& operator << (std::ostream& os, const AmbientLight& l)
{
	return l.prettyPrint(os);
}

std::ostream& AmbientLight::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (AmbientLight) intensity: " << intensity << " }";
}

} // namespace gintonic
