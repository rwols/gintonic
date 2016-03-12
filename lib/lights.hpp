/**
 * @file lights.hpp
 * @brief Defines the light base class component and also defines all derived
 * light classes.
 * @author Raoul Wols
 */

#pragma once

#include "Object.hpp"
#include "vec4f.hpp"
#include "mat4f.hpp"
#include "SQT.hpp"
#include "opengl/framebuffer.hpp"
#include "opengl/texture_object.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>
#include <map>

#include <fbxsdk.h>

namespace gintonic {

class entity; // Forward declaration.
class proj_info; // Forward declaration.

/**
 * @brief A light. Virtual base component for inheritance.
 */
class Light : public Object<Light>
{
public:

	static std::shared_ptr<Light> create(const FbxLight*);

	/**
	 * @brief The intensity of the light.
	 * 
	 * @details The first three components, the X, Y and Z values, define the
	 * color of the light. The fourth component, the W value, defines the
	 * intensity of the light.
	 */
	vec4f intensity;

	// virtual void attach(entity&);

	// virtual void detach(entity&);

	/// Default constructor.
	Light() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	Light(const vec4f& intensity);
	
	/// Destructor.
	virtual ~Light() noexcept = default;
	
	/**
	 * @brief Shine the light using the global transform of the given entity.
	 * @details Note that the renderer must be in the light pass stage.
	 * 
	 * @param e The entity to shine from.
	 */
	virtual void shine(const entity& e) const noexcept = 0;

	virtual void beginShadowPass(const entity& light_ent)
	{
		/* Put me in a CPP file */
	}

	virtual void renderShadow(const entity& geometry) const noexcept
	{
		/* Put me in a CPP file */
	}

	/**
	 * @brief Set the brightness, or intensity of the light.
	 * 
	 * @param brightness The new brightness value.
	 */
	virtual void setBrightness(const float brightness);

	/**
	 * @brief Get the brightness, or intensity of the light.
	 * @return The current brightness, or intensity.
	 */
	float brightness() const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const = 0;

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const Light*);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const std::unique_ptr<Light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const std::shared_ptr<Light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const Light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	/**
	 * @brief Print the light to an outstream for debug purposes.
	 */
	virtual std::ostream& prettyPrint(std::ostream&) const 
		noexcept;

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(intensity);
	}
};

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
	 * entity.
	 *
	 * @details In the case of an ambient light, no transformation information
	 * is used at all. Every surface is lit evenly in the whole scene.
	 *
	 * @param e The entity to use. Note that the global transformation is
	 * used.
	 */
	virtual void shine(const entity& e) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const
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
	 * @brief Shine the light given the entity's global transformation.
	 *
	 * @details In the case of a directional light, only the rotational part
	 * of the global transformation is used, since a directional light has
	 * no intrinsic position. A directional light only cares about its own
	 * direction and the normal vector of the surface to light.
	 *
	 * @param e The entity to use the global transform of.
	 */
	virtual void shine(const entity& e) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const;

	virtual void beginShadowPass(const entity& light_ent);

	virtual void renderShadow(const entity& geometry) const noexcept;

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

/**
 * @brief A point light.
 *
 * @details A point light has an intrinsic position, but no idea of a
 * direction. A point light is a point in the scene that casts light in
 * every direction. A point light also carries an extra datamember called the
 * attenuation. Indeed, we must have some way to model the falloff of a point
 * light depending on the distance to the point light. This could be a
 * constant falloff, a linear falloff or a quadratic falloff (or higher
 * powers). It can also be a combination. The way we model it is as follows.
 * Let \f$d \geq 0\f$ be the distance from the point light to the surface to
 * light. Suppose that the attenuation value is \f$(a_x, a_y, a_z)\f$. Then
 * the light contribution is multiplied by a factor of
 * \f$ 1/ \left( a_x + a_y d + a_z d^2 \right) \f$.
 */
class PointLight : public Light
{
public:

	/// Default constructor.
	PointLight() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	PointLight(const vec4f& intensity);
	
	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 * @param attenuation The attenuation value.
	 */
	PointLight(const vec4f& intensity, const vec4f& attenuation);

	/// Destructor.
	virtual ~PointLight() noexcept = default;

	/**
	 * @brief Get the attenuation.
	 * @return The attenuation value.
	 */
	vec4f attenuation() const noexcept;

	// Set the attenuation value. Setting a new attenuation value
	// also recalculates the cutoff radius (see below).
	
	/**
	 * @brief Set the attenuation value.
	 *
	 * @details When the attenuation value is set, the cutoff point is
	 * recalculated.
	 * 
	 * @param a The attenuation value.
	 */
	void setAttenuation(const vec4f& a) noexcept;
	
	/**
	 * @brief Get the cutoff radius.
	 *
	 * @details The cutoff radius is the value where the light intensity of
	 * the point light is so diminished that pixels do not get any brighter.
	 * 
	 * @return The cutoff radius.
	 */
	float cutoffPoint() const noexcept;

	/**
	 * @brief Shine the point light given the entity's global transform.
	 *
	 * @details In the case of a point light, only the translation part of
	 * the global transformation matrix of the entity is used.
	 */
	virtual void shine(const entity&) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const;

	/**
	 * @brief Set the brightness.
	 *
	 * @details When the brightness value is set, the cutoff point is
	 * recalculated.
	 * 
	 * @param brightness The brightness value.
	 */
	virtual void setBrightness(const float brightness);

	/// Stream output support for a point light.
	friend std::ostream& operator << (std::ostream&, const PointLight&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	vec4f mAttenuation;

	float mCutoffPoint;

	void calculateCutoffRadius() noexcept;

	// Reimplement this method to support output streams.
	virtual std::ostream& prettyPrint(std::ostream&) const 
		noexcept;

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive>
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Light);
		ar & boost::serialization::make_nvp("attenuation", mAttenuation);
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		vec4f att;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Light);
		ar & boost::serialization::make_nvp("attenuation", att);
		setAttenuation(att);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

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
	 * entity.
	 *
	 * @details In the case of a spot light, both the rotational part as well
	 * as the translation part of the global transformation of the entity is
	 * used.
	 *
	 * @param e The entity to use for the global transformation matrix.
	 */
	virtual void shine(const entity& e) const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<entity> lightEntity) const;

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
