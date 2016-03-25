#pragma once

#include "../ForwardDeclarations.hpp"
#include <vector>
#include <memory>

namespace gintonic {

/**
 * @brief Encapsulates a scratch pad for shadows.
 */
class ShadowBuffer
{
public:

	/// Destructor.
	virtual ~ShadowBuffer() noexcept = default;
	
	/**
	 * @brief Draw the shadow map.
	 * @param lightEntity The light entity that houses this ShadowBuffer.
	 * @param shadowCastingGeometryEntities An array of geometry entities
	 * that cast shadows.
	 */
	virtual void collect(
		Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept = 0;

	/**
	 * @brief Bind the depth textures that recorded the shadows.
	 */
	virtual void bindDepthTextures() const noexcept = 0;

	/**
	 * @brief Get the current projection matrix that was used
	 * during the call to ShadowBuffer::collect.
	 * @return The projection matrix.
	 */
	virtual const mat4f& projectionMatrix() const noexcept = 0;
};

} // namespace gintonic