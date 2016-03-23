#include "GeometryBuffer.hpp"

namespace gintonic {

constexpr GLenum sTextureInternal[GeometryBuffer::kCount] = 
{
	GL_RGBA32F,          // kPosition
	GL_RGBA,             // kDiffuse
	GL_RGBA,             // kSpecular
	GL_RGBA16F,          // kNormal
	GL_RGB,              // kPostProcessing
	GL_DEPTH32F_STENCIL8 // kDepth
};

constexpr GLenum sTextureFormat[GeometryBuffer::kCount] = 
{
	GL_RGBA,         // kPosition
	GL_RGBA,         // kDiffuse
	GL_RGBA,         // kSpecular
	GL_RGBA,         // kNormal
	GL_RGB,          // kPostProcessing
	GL_DEPTH_STENCIL // kDepth
};

constexpr GLenum sTextureType[GeometryBuffer::kCount] =
{
	GL_FLOAT,                         // kPosition
	GL_UNSIGNED_BYTE,                 // kDiffuse
	GL_UNSIGNED_BYTE,                 // kSpecular
	GL_FLOAT,                         // kNormal
	GL_UNSIGNED_BYTE,                 // kPostProcessing
	GL_FLOAT_32_UNSIGNED_INT_24_8_REV // kDepth
};

constexpr GLenum sAttachment[GeometryBuffer::kCount] =
{
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kPosition,       // kPosition
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kDiffuse,        // kDiffuse
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kSpecular,       // kSpecular
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kNormal,         // kNormal
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kPostProcessing, // kPostProcessing
	GL_DEPTH_STENCIL_ATTACHMENT                             // kDepth
};

constexpr GLenum sDrawBuffers[GeometryBuffer::kPostProcessing] = 
{
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kPosition, 
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kDiffuse, 
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kSpecular, 
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kNormal
}; 

GeometryBuffer::GeometryBuffer(const int width, const int height)
{
	resize(width, height);
}

void GeometryBuffer::resize(const int width, const int height)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer);
	for (unsigned int i = 0 ; i < kCount; ++i) 
	{
		glBindTexture(GL_TEXTURE_2D, mTextures[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, sTextureInternal[i], width, height, 0, sTextureFormat[i], sTextureType[i], nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, sAttachment[i], GL_TEXTURE_2D, mTextures[i], 0);
	}
	mFramebuffer.checkStatus();
}

void GeometryBuffer::prepareGeometryPhase() noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer);
	glDrawBuffers(kPostProcessing, sDrawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GeometryBuffer::prepareLightingPhase() noexcept
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + kPostProcessing);
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int i = 0; i < kPostProcessing; ++i) mTextures[i].bind(GL_TEXTURE_2D, i);
}

void GeometryBuffer::preparePostProcessingPhase() noexcept
{
	mTextures[kPostProcessing].bind(GL_TEXTURE_2D, 0);
}

void GeometryBuffer::blitDrawbuffersToPostProcessingBuffer(const int width, const int height) noexcept
{
	const GLsizei halfwidth = (GLsizei)(width / 2.0f);
	const GLsizei halfheight = (GLsizei)(height / 2.0f);

	glDrawBuffer(GL_COLOR_ATTACHMENT0 + kPostProcessing);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kPosition);
	glBlitFramebuffer(0, 0, width, height, 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kNormal);
	glBlitFramebuffer(0, 0, width, height, 0, halfheight, halfwidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kDiffuse);
	glBlitFramebuffer(0, 0, width, height, halfwidth, halfheight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kSpecular);
	glBlitFramebuffer(0, 0, width, height, halfwidth, 0, width, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GeometryBuffer::finalize(const int width, const int height) noexcept
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // <-- The screen
	glReadBuffer(GL_COLOR_ATTACHMENT0 + kPostProcessing);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

} // namespace gintonic