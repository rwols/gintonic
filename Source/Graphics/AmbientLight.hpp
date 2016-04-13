#pragma once

#include "Light.hpp"

namespace gintonic {

/**
 * @brief An ambient light.
 *
 * @details Ambient lights are an extremely simple type of light. They do not
 * care in what direction the surface to light is or what the distance to the
 * surface to light is. Everything in the world is evenly lit by an ambient
 * light. It is probably sufficient to have exactly one ambient light per
 * scene.
 */
class AmbientLight : public Light
{
public:

	/// Default constructor.
	AmbientLight() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	AmbientLight(const vec4f& intensity);

	/// Destructor.
	virtual ~AmbientLight() noexcept = default;
	
	virtual void shine(
		const Entity& lightEntity, 
		const std::vector<std::shared_ptr<Entity>>& shadowCastingGeometryEntities) const noexcept;

	/**
	 * @brief Get the attenuation value. This method
	 * always returns the vector `[0,0,0,0]` in the case of an AmbientLight.
	 * @return The vector `[0,0,0,0]`
	 */
	inline virtual vec4f getAttenuation() const noexcept
	{
		return vec4f(0.0f, 0.0f, 0.0f, 0.0f);
	}

	/**
	 * @brief Set the attenuation value. This method is a no-op in the
	 * case of an AmbientLight.
	 * @param [in] attenuation The new attenuation value. This method
	 * does nothing with the parameter.
	 */
	inline virtual void setAttenuation(const vec4f& attenuation)
	{
		/* do nothing */
	}

	/**
	 * @brief Get the cosine of the half angle. This method always
	 * returns `0` in the case of an AmbientLight.
	 * @return The cosine of the half angle. Always `0` in the
	 * case of an AmbientLight.
	 */
	inline virtual float getCosineHalfAngle() const noexcept
	{
		return 0.0f;
	}

	/**
	 * @brief Set the cosine of the half angle. This must be a
	 * value in the closed interval `[0,1]`. This is a no-op in the
	 * case of an AmbientLight.
	 * @param [in] cosineHalfAngle The cosine of the half angle. This
	 * method does nothing with the parameter.
	 */
	inline virtual void setCosineHalfAngle(const float cosineHalfAngle)
	{
		/* do nothing */
	}

	/**
	 * @brief Initialize the ShadowBuffer for the given Entity.
	 * An AmbientLight does nothing at all and this method is a no-op.
	 * @param lightEntity A mutable reference to the Entity that will
	 * get a non-trivial ShadowBuffer. An AmbientLight does nothing
	 * at all and this method is a no-op.
	 */
	virtual void initializeShadowBuffer(Entity& lightEntity) const
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Output stream support.
	 */
	friend std::ostream& operator << (std::ostream&, const AmbientLight&);

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	virtual std::ostream& prettyPrint(std::ostream&) const noexcept;

	friend boost::serialization::access;
	
	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Light);
	}
};

} // namespace gintonic

