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

	/*
	 * @brief Shine the light given the Entity's global transformation.
	 *
	 * @details In the case of a directional light, only the rotational part
	 * of the global transformation is used, since a directional light has
	 * no intrinsic position. A directional light only cares about its own
	 * direction and the normal vector of the surface to light.
	 *
	 * @param e The Entity to use the global transform of.
	 */
	virtual void shine(const Entity& e) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const;

	virtual void beginShadowPass(const Entity& light_ent);

	virtual void renderShadow(const Entity& geometry) const noexcept;

	/// Stream output support for a directional light.
	friend std::ostream& operator << (std::ostream&, const DirectionalLight&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& prettyPrint(std::ostream&) const 
		noexcept;

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AmbientLight);
	}
};

} // namespace gintonic