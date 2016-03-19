#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class PointShadowBuffer : public ShadowBuffer
{
public:
	PointShadowBuffer();
	virtual ~PointShadowBuffer() noexcept = default;
	virtual void bindForWriting() const noexcept;
	virtual void bindForReading() const noexcept;
private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
	// mutable int m_face = 0;
};

} // namespace gintonic 