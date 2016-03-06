#include "proj_info.hpp"
#include "renderer.hpp"
#include "entity.hpp"

namespace gintonic {

// void proj_info::attach(entity& e)
// {
// 	if (e.m_proj_info_component == this) return;
// 	else if (e.m_proj_info_component) e.m_proj_info_component->detach(e);
// 	e.m_proj_info_component = this;
// 	m_ents.push_back(&e);
// }

// void proj_info::detach(entity& e)
// {
// 	for (auto i = begin(); i != end(); ++i)
// 	{
// 		if (*i == &e)
// 		{
// 			e.m_proj_info_component = nullptr;
// 			m_ents.erase(i);
// 			return;
// 		}
// 	}
// }

void proj_info::update() noexcept
{
	// fieldofview = 60.0f;
	switch (projection)
	{
		case kOrthographicProjection:
			projection_matrix.set_orthographic(
				width, 
				height, 
				nearplane,
				farplane);
			break;
		case kPerspectiveProjection:
			projection_matrix.set_perspective(
				fieldofview, 
				renderer::aspectratio(), 
				nearplane, 
				farplane);
			break;
		default:
			projection_matrix.set_perspective(
				fieldofview, 
				renderer::aspectratio(), 
				nearplane, 
				farplane);
	}
}

} // namespace gintonic