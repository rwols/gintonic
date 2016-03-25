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
	
	/**
	 * @brief Shine the light using the global transformation of the given
	 * Entity.
	 *
	 * @details In the case of an ambient light, no transformation information
	 * is used at all. Every surface is lit evenly in the whole scene.
	 *
	 * @param e The Entity to use. Note that the global transformation is
	 * used.
	 */
	virtual void shine(const Entity& e) const noexcept;

	inline virtual vec4f getAttenuation() const noexcept
	{
		return vec4f(0.0f, 0.0f, 0.0f, 0.0f);
	}

	inline virtual void setAttenuation(const vec4f&)
	{
		/* do nothing */
	}

	inline virtual float getCosineHalfAngle() const noexcept
	{
		return 0.0f;
	}

	inline virtual void setCosineHalfAngle(const float angle)
	{
		/* do nothing */
	}

	virtual void initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const
	{
		/* Empty on purpose. */
	}

	/**
	 * @brief Output stream support.
	 */
	friend std::ostream& operator << (std::ostream&, const AmbientLight&);

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
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Light);
	}
};

} // namespace gintonic