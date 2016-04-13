#pragma once

#include "AmbientLight.hpp"

namespace gintonic {

/**
 * @brief A directional light.
 *
 * @details Directional lights are one step above ambient lights in
 * complexity. They have no intrinsic position, and the only thing that a
 * directional light cares about is its own direction and the surface normal
 * of the surface to light. Directional lights give the illusion of some light
 * that is very far away. For instance, the sun. In reality, the sun's light
 * rays do not actually have only one direction, but since the sun is so large
 * compared to the surface of the earth, a very good approximation of the
 * sun's rays is to assume they are in fact uniform in one direction.
 */
class DirectionalLight : public AmbientLight
{
public:

	/// Default constructor.
	DirectionalLight() = default;
	
	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	DirectionalLight(const vec4f& intensity);

	/// Destructor.
	virtual ~DirectionalLight() noexcept = default;
	
	virtual void shine(
		const Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) const noexcept;

	virtual void initializeShadowBuffer(Entity& lightEntity) const;

	/// Stream output support for a directional light.
	friend std::ostream& operator << (std::ostream&, const DirectionalLight&);

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& prettyPrint(std::ostream&) const 
		noexcept;

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AmbientLight);
	}
};

} // namespace gintonic