#include "camera.hpp"
#include "mat4f.hpp"
#include "entity.hpp"

namespace gintonic {

void camera::attach(entity& e)
{
	if (e.m_camera_component == this) return;
	else if (e.m_camera_component) e.m_camera_component->detach(e);
	e.m_camera_component = this;
	m_ents.push_back(&e);
}

void camera::detach(entity& e)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (*i == &e)
		{
			e.m_camera_component = nullptr;
			m_ents.erase(i);
			return;
		}
	}
}

camera::camera(entity& e)
{
	if (e.m_camera_component) e.m_camera_component->detach(e);
	e.m_camera_component = this;
	m_ents.push_back(&e);
}

#define M_PIf2 static_cast<float>(M_PI) * 0.5f
#define M_2PIf static_cast<float>(M_PI) * 2.0f

void camera::add_mouse(const vec2f& amount) noexcept
{
	angles += amount;

	angles.x = std::remainder(angles.x, M_2PIf);

	if (angles.y < -M_PIf2) angles.y = -M_PIf2;
	else if (angles.y > M_PIf2) angles.y = M_PIf2;

	for (auto e = begin(); e != end(); ++e) (*e)->set_rotation(quatf::mouse(angles));
}

camera::~camera()
{
	for (auto* e : m_ents) e->m_camera_component = nullptr;
}

} // namespace gintonic