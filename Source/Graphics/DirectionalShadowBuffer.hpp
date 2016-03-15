#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class DirectionalShadowBuffer : public ShadowBuffer
{
public:
	DirectionalShadowBuffer();
	virtual ~DirectionalShadowBuffer() noexcept = default;
	virtual void bindForWriting() const noexcept;
	virtual void bindForReading() const noexcept;
private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
};

} // namespace gintonic