
#pragma once

#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

class GeometryBuffer
{
public:

	enum BufferType
	{
		kPosition = 0,
		kDiffuse,
		kSpecular,
		kNormal,
		kPostProcessing,
		kDepth,
		kCount
	};

	// Can throw OpenGL::Framebuffer::IncompleteException
	GeometryBuffer(const int width, const int height);

	void resize(const int width, const int height);

	void prepareGeometryPhase() noexcept;
	void prepareLightingPhase() noexcept;
	void preparePostProcessingPhase() noexcept;
	void blitDrawbuffersToPostProcessingBuffer(const int width, const int height) noexcept;
	void finalize(const int width, const int height) noexcept;

private:
	OpenGL::Framebuffer mFramebuffer;
	OpenGL::TextureObject mTextures[kCount];
};

} // namespace gintonic