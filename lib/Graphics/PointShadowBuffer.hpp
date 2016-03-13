#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/framebuffer.hpp"
#include "OpenGL/texture_object.hpp"

namespace gintonic {

class PointShadowBuffer : public ShadowBuffer
{
public:
	PointShadowBuffer();
	virtual ~PointShadowBuffer() noexcept = default;
	virtual void bindForWriting() const noexcept;
	virtual void bindForReading() const noexcept;
private:
	opengl::framebuffer mFramebuffer;
	opengl::texture_object mTexture;
	mutable int m_face = 0;
};

} // namespace gintonic 