/**
 * @file lights.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_lights_hpp
#define gintonic_lights_hpp

#include "component.hpp"
#include "vec4f.hpp"
#include "SQT.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

class entity; // Forward declaration.

/**
 * @brief A light. Virtual base component for inheritance.
 */
class light : public component
{
public:

	/**
	 * @brief The intensity of the light.
	 * 
	 * @details The first three components, the X, Y and Z values, define the
	 * color of the light. The fourth component, the W value, defines the
	 * intensity of the light.
	 */
	vec4f intensity;

	virtual void attach(entity&) final;

	virtual void detach(entity&) final;

	/// Default constructor.
	light() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	light(const vec4f& intensity);
	
	/// Destructor.
	virtual ~light() BOOST_NOEXCEPT_OR_NOTHROW;
	
	/**
	 * @brief Shine the light using the global transform of the given entity.
	 * @details Note that the renderer must be in the light pass stage.
	 * 
	 * @param e The entity to shine from.
	 */
	virtual void shine(const entity& e) const BOOST_NOEXCEPT_OR_NOTHROW = 0;

	/**
	 * @brief Set the brightness, or intensity of the light.
	 * 
	 * @param brightness The new brightness value.
	 */
	virtual void set_brightness(const float brightness);

	/**
	 * @brief Get the brightness, or intensity of the light.
	 * @return The current brightness, or intensity.
	 */
	float brightness() const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const light*);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const std::unique_ptr<light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const std::shared_ptr<light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	/**
	 * @brief Print the light to an outstream for debug purposes.
	 */
	virtual std::ostream& pretty_print(std::ostream&) const 
		BOOST_NOEXCEPT_OR_NOTHROW;

	friend boost::serialization::access;

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
class ambient_light : public light
{
public:

	/// Default constructor.
	ambient_light() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	ambient_light(const vec4f& intensity);

	/// Destructor.
	virtual ~ambient_light() BOOST_NOEXCEPT_OR_NOTHROW;
	
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
	virtual void shine(const entity& e) const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Output stream support.
	 */
	friend std::ostream& operator << (std::ostream&, const ambient_light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const 
		BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(light);
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
class directional_light : public ambient_light
{
public:

	/// Default constructor.
	directional_light() = default;
	
	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	directional_light(const vec4f& intensity);

	/// Destructor.
	virtual ~directional_light() BOOST_NOEXCEPT_OR_NOTHROW;

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
	virtual void shine(const entity& e) const BOOST_NOEXCEPT_OR_NOTHROW;

	/// Stream output support for a directional light.
	friend std::ostream& operator << (std::ostream&, const directional_light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const 
		BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ambient_light);
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
class point_light : public light
{
public:

	/// Default constructor.
	point_light() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	point_light(const vec4f& intensity);
	
	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 * @param attenuation The attenuation value.
	 */
	point_light(const vec4f& intensity, const vec4f& attenuation);

	/// Destructor.
	virtual ~point_light() BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Get the attenuation.
	 * @return The attenuation value.
	 */
	vec4f attenuation() const BOOST_NOEXCEPT_OR_NOTHROW;

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
	void set_attenuation(const vec4f& a) BOOST_NOEXCEPT_OR_NOTHROW;
	
	/**
	 * @brief Get the cutoff radius.
	 *
	 * @details The cutoff radius is the value where the light intensity of
	 * the point light is so diminished that pixels do not get any brighter.
	 * 
	 * @return The cutoff radius.
	 */
	float cutoff_point() const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Shine the point light given the entity's global transform.
	 *
	 * @details In the case of a point light, only the translation part of
	 * the global transformation matrix of the entity is used.
	 */
	virtual void shine(const entity&) const BOOST_NOEXCEPT_OR_NOTHROW;

	/**
	 * @brief Set the brightness.
	 *
	 * @details When the brightness value is set, the cutoff point is
	 * recalculated.
	 * 
	 * @param brightness The brightness value.
	 */
	virtual void set_brightness(const float brightness);

	/// Stream output support for a point light.
	friend std::ostream& operator << (std::ostream&, const point_light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	vec4f m_attenuation;

	float m_cutoff_point;

	void calculate_cutoff_radius() BOOST_NOEXCEPT_OR_NOTHROW;

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const 
		BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned /*version*/) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(light);
		ar & boost::serialization::make_nvp("attenuation", m_attenuation);
	}

	template <class Archive>
	void load(Archive& ar, const unsigned /*version*/)
	{
		vec4f att;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(light);
		ar & boost::serialization::make_nvp("attenuation", att);
		set_attenuation(att);
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
class spot_light : public point_light
{
public:

	/// Default constructor.
	spot_light() = default;

	/**
	 * @brief Constructor.
	 *
	 * @param intensity The intensity value.
	 */
	spot_light(const vec4f& intensity);
	
	/**
	 * @brief Constructor.
	 *
	 * @param intensity The intensity value.
	 * @param attenuation The attenuation value.
	 */
	spot_light(const vec4f& intensity, const vec4f& attenuation);

	/// Destructor.
	virtual ~spot_light() BOOST_NOEXCEPT_OR_NOTHROW;
	
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
	virtual void shine(const entity& e) const BOOST_NOEXCEPT_OR_NOTHROW;

	/// Stream output support for a spot light.
	friend std::ostream& operator << (std::ostream&, const spot_light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(point_light);
	}
};

} // namespace gintonic

#endif