#include "SpotShadowBuffer.hpp"
#include "../Foundation/exception.hpp"

namespace gintonic {

SpotShadowBuffer::SpotShadowBuffer()
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void SpotShadowBuffer::bindForWriting() const noexcept
{
	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
}

void SpotShadowBuffer::bindForReading() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, 0);
}

} // namespace gintonic