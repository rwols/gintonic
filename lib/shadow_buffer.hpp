#pragma once

#include "opengl/framebuffer.hpp"
#include "opengl/texture_object.hpp"

namespace gintonic {

class shadow_buffer
{
public:
	virtual ~shadow_buffer() noexcept = default;
	virtual void bind_for_writing() const noexcept = 0;
	virtual void bind_for_reading() const noexcept = 0;
};

class directional_shadow_buffer : public shadow_buffer
{
public:
	directional_shadow_buffer();
	virtual ~directional_shadow_buffer() noexcept = default;
	virtual void bind_for_writing() const noexcept;
	virtual void bind_for_reading() const noexcept;
private:
	opengl::framebuffer m_framebuffer;
	opengl::texture_object m_texture;
};

class point_shadow_buffer : public shadow_buffer
{
public:
	point_shadow_buffer();
	virtual ~point_shadow_buffer() noexcept = default;
	virtual void bind_for_writing() const noexcept;
	virtual void bind_for_reading() const noexcept;
private:
	opengl::framebuffer m_framebuffer;
	opengl::texture_object m_texture;
	mutable int m_face = 0;
};

} // namespace gintonic