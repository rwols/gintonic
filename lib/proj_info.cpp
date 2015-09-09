#include "proj_info.hpp"
#include "renderer.hpp"
#include "entity.hpp"

namespace gintonic {

void proj_info::attach(entity& e)
{
	if (e.m_proj_info_component == this) return;
	else if (e.m_proj_info_component) e.m_proj_info_component->detach(e);
	e.m_proj_info_component = this;
	m_ents.push_back(&e);
}

void proj_info::detach(entity& e)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (*i == &e)
		{
			e.m_proj_info_component = nullptr;
			m_ents.erase(i);
			return;
		}
	}
}

void proj_info::update() BOOST_NOEXCEPT_OR_NOTHROW
{
	// fieldofview = 60.0f;
	switch (projection)
	{
		case kOrthographicProjection:
			matrix.set_orthographic(
				static_cast<float>(renderer::width()), 
				static_cast<float>(renderer::height()), 
				0.1f, 
				100.0f);
			break;
		case kPerspectiveProjection:
			matrix.set_perspective(
				fieldofview, 
				renderer::aspectratio(), 
				0.1f, 
				100.0f);
			break;
		default:
			matrix.set_perspective(
				fieldofview, 
				renderer::aspectratio(), 
				0.1f, 
				100.0f);
	}
}

proj_info::~proj_info()
{
	for (auto* e : m_ents) e->m_proj_info_component = nullptr;
}

} // namespace gintonic