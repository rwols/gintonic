#pragma once
#include "ShadowBuffer.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class SpotShadowBuffer : public ShadowBuffer
{
public:
	SpotShadowBuffer();
	virtual ~SpotShadowBuffer() noexcept = default;
	virtual void bindForWriting() const noexcept;
	virtual void bindForReading() const noexcept;
private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTexture;
};

} // namespace gintonic