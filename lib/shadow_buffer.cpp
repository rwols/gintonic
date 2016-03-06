#include "shadow_buffer.hpp"

directional_shadow_buffer::directional_shadow_buffer()
{
	// TODO
}

void directional_shadow_buffer::bind_for_writing() const noexcept
{
	m_framebuffer.bind(GL_DRAW_FRAMEBUFFER);
}

void directional_shadow_buffer::bind_for_reading() const noexcept
{
	m_texture.bind(GL_TEXTURE0);
}

