#include "GeometryBuffer.hpp"

#include "Renderer.hpp"
#include "ShaderPrograms.hpp"
#include "Mesh.hpp"
#include "../Entity.hpp"
#include "../Camera.hpp"

namespace gintonic {

constexpr GLenum sTextureInternal[GeometryBuffer::kCount] = 
{
	GL_RGBA32F,          // kPosition
	GL_RGBA,             // kDiffuse
	GL_RGBA,             // kSpecular
	GL_RGBA16F,          // kNormal
	GL_RGB,              // kPostProcessing
	GL_DEPTH_COMPONENT   // kDepth
};

constexpr GLenum sTextureFormat[GeometryBuffer::kCount] = 
{
	GL_RGBA,           // kPosition
	GL_RGBA,           // kDiffuse
	GL_RGBA,           // kSpecular
	GL_RGBA,           // kNormal
	GL_RGB,            // kPostProcessing
	GL_DEPTH_COMPONENT // kDepth
};

constexpr GLenum sTextureType[GeometryBuffer::kCount] =
{
	GL_FLOAT,         // kPosition
	GL_UNSIGNED_BYTE, // kDiffuse
	GL_UNSIGNED_BYTE, // kSpecular
	GL_FLOAT,         // kNormal
	GL_UNSIGNED_BYTE, // kPostProcessing
	GL_FLOAT          // kDepth
};

constexpr GLenum sAttachment[GeometryBuffer::kCount] =
{
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kPosition,       // kPosition
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kDiffuse,        // kDiffuse
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kSpecular,       // kSpecular
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kNormal,         // kNormal
	GL_COLOR_ATTACHMENT0 + GeometryBuffer::kPostProcessing, // kPostProcessing
	GL_DEPTH_ATTACHMENT                                     // kDepth
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

void GeometryBuffer::prepareGeometryPhase() const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer);
	glDrawBuffers(kPostProcessing, sDrawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GeometryBuffer::prepareLightingPhase() const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + kPostProcessing);
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int i = 0; i < kPostProcessing; ++i) mTextures[i].bind(GL_TEXTURE_2D, i);
}

void GeometryBuffer::preparePostProcessingPhase() const noexcept
{
	mTextures[kPostProcessing].bind(GL_TEXTURE_2D, 0);
}

void GeometryBuffer::blitDrawbuffersToScreen(const int width, const int height) const noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer);

	const GLsizei halfwidth = (GLsizei)(width / 2.0f);
	const GLsizei halfheight = (GLsizei)(height / 2.0f);

	// glDrawBuffer(GL_COLOR_ATTACHMENT0 + kPostProcessing);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kPosition);
	glBlitFramebuffer(0, 0, width, height, 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kNormal);
	glBlitFramebuffer(0, 0, width, height, 0, halfheight, halfwidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kDiffuse);
	glBlitFramebuffer(0, 0, width, height, halfwidth, halfheight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + kSpecular);
	glBlitFramebuffer(0, 0, width, height, halfwidth, 0, width, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GeometryBuffer::bindDepthTexture(const GLint textureUnit) const noexcept
{
	mTextures[kDepth].bind(GL_TEXTURE_2D, textureUnit);
}

void GeometryBuffer::finalize(const int width, const int height) const noexcept
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // <-- The screen
	glReadBuffer(GL_COLOR_ATTACHMENT0 + kPostProcessing);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

} // namespace gintonic