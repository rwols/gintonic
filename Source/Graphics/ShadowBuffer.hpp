#pragma once

#include "../ForwardDeclarations.hpp"
#include <vector>
#include <memory>

namespace gintonic {

class ShadowBuffer
{
public:
	virtual ~ShadowBuffer() noexcept = default;
	
	virtual void collect(
		Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) noexcept = 0;

	virtual void bindDepthTextures() const noexcept = 0;
};

} // namespace gintonic