/**
 * @file GeometryBuffer.hpp
 * @author Raoul Wols
 * @brief Defines the GeometryBuffer class.
 */

#pragma once

#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/TextureObject.hpp"

namespace gintonic {

/**
 * @brief Handles the various buffers during geometry collection.
 */
class GeometryBuffer
{
public:

	/**
	 * The various texture or buffer types.
	 */
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

	/**
	 * @brief Constructs a new OpenGL::Framebuffer with
	 * five textures attached. The supplied width and height
	 * should be the width and height of the renderer.
	 * @param [in] width The width of the textures.
	 * @param [in] height The height of the textures.
	 * @throws OpenGL::Framebuffer::Exception when the OpenGL::Framebuffer
	 * fails to initialize.
	 */
	GeometryBuffer(const int width, const int height);

	/**
	 * @brief Resize the textures attached to the OpenGL::Framebuffer.
	 * @param [in] width The width of the textures.
	 * @param [in] height The height of the textures.
	 * @throws OpenGL::Framebuffer::Exception when the OpenGL::Framebuffer
	 * fails to resize.
	 */
	void resize(const int width, const int height);

	/**
	 * @brief Bind the OpenGL::Framebuffer for drawing
	 * into the first four textures.
	 */
	void prepareGeometryPhase() const noexcept;

	/**
	 * @brief Bind the OpenGL::Framebuffer to drawing
	 * into the fifth texture, and bind the first four
	 * textures into texture units.
	 */
	void prepareLightingPhase() const noexcept;

	/**
	 * @brief Bind the default framebuffer and bind
	 * the fifth texture into texture unit zero.
	 */
	void preparePostProcessingPhase() const noexcept;

	/**
	 * @brief Set up the GeometryBuffer for stencil
	 * pass tests. This assumes you are in the lighting
	 * phase.
	 */
	void beginStencilPass() const noexcept;

	/**
	 * @brief Restore the state of the GeometryBuffer
	 * back to the normal lighting phase.
	 */
	void endStencilPass() const noexcept;

	/**
	 * @brief Blit the first four textures to the screen.
	 * @param [in] width The renderer width.
	 * @param [in] height The renderer height.
	 */
	void blitDrawbuffersToScreen(const int width, const int height) const noexcept;

	/**
	 * @brief Blit the contents of the fifth texture
	 * to the default framebuffer so that we see the results.
	 * @param [in] width The renderer width.
	 * @param [in] height The renderer height.
	 */
	void finalize(const int width, const int height) const noexcept;

	/**
	 * @brief Bind the depth texture of the geometry buffer.
	 * @param [in] textureUnit The texture unit.
	 */
	void bindDepthTexture(const GLint textureUnit) const noexcept;

private:

	OpenGL::Framebuffer mFramebuffer;

	OpenGL::TextureObject mTextures[kCount];
};

} // namespace gintonic