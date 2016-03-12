#include "DirectionalShadowBuffer.hpp"
#include "exception.hpp"

namespace gintonic {

DirectionalShadowBuffer::DirectionalShadowBuffer()
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void DirectionalShadowBuffer::bindForWriting() const noexcept
{
	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
}

void DirectionalShadowBuffer::bindForReading() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, 0);
}

} // namespace gintonic