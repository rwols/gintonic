#pragma once
#include "ShadowBuffer.hpp"
#include "opengl/framebuffer.hpp"
#include "opengl/texture_object.hpp"

namespace gintonic {

class DirectionalShadowBuffer : public ShadowBuffer
{
public:
	DirectionalShadowBuffer();
	virtual ~DirectionalShadowBuffer() noexcept = default;
	virtual void bindForWriting() const noexcept;
	virtual void bindForReading() const noexcept;
private:
	opengl::framebuffer mFramebuffer;
	opengl::texture_object mTexture;
};

} // namespace gintonic