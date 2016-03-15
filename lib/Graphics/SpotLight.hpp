#pragma once

#include "PointLight.hpp"

namespace gintonic {

/**
 * @brief A spot light.
 *
 * @brief A spot light is like a point light, but it also has a preferred
 * direction of lighting. In some sense, a spot light is a mix between a
 * directional light and a point light. It has both a position and a
 * direction. Just as with a a point light, it also carries an attenuation
 * value.
 */
class SpotLight : public PointLight
{
public:

	/// Default constructor.
	SpotLight() = default;

	/**
	 * @brief Constructor.
	 *
	 * @param intensity The intensity value.
	 */
	SpotLight(const vec4f& intensity);
	
	/**
	 * @brief Constructor.
	 *
	 * @param intensity The intensity value.
	 * @param attenuation The attenuation value.
	 */
	SpotLight(const vec4f& intensity, const vec4f& attenuation);

	/// Destructor.
	virtual ~SpotLight() noexcept = default;
	
	// Shine the spot light given the SQT transform. The spot light
	// uses both the rotation part as well as the translation (position)
	// part of the SQT transform. It does nothing with the scale part.
	
	/**
	 * @brief Shine the spot light given the global transformation of the
	 * Entity.
	 *
	 * @details In the case of a spot light, both the rotational part as well
	 * as the translation part of the global transformation of the Entity is
	 * used.
	 *
	 * @param e The Entity to use for the global transformation matrix.
	 */
	virtual void shine(const Entity& e) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const;

	/// Stream output support for a spot light.
	friend std::ostream& operator << (std::ostream&, const SpotLight&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& prettyPrint(std::ostream&) const noexcept;

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(PointLight);
	}
};

} // namespace gintonic