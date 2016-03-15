#include "PointShadowBuffer.hpp"
#include "../Foundation/exception.hpp"

namespace gintonic {

PointShadowBuffer::PointShadowBuffer()
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void PointShadowBuffer::bindForWriting() const noexcept
{
	mFramebuffer.bind(GL_DRAW_FRAMEBUFFER);
}

void PointShadowBuffer::bindForReading() const noexcept
{
	mTexture.bind(GL_TEXTURE_2D, 0);
}

} // namespace gintonic