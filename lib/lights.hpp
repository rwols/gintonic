#ifndef gintonic_lights_hpp
#define gintonic_lights_hpp

#include "vec4f.hpp"
#include "SQT.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

/*****************************************************************************
* gintonic::light                                                            *
*****************************************************************************/

class light : public std::enable_shared_from_this<light>
{
public:

	// Light intensity property.
	vec4f intensity;

	// Default constructor.
	light() = default;

	// Constructor that takes an intensity value.
	light(const vec4f& intensity);
	
	// Destructor.
	virtual ~light() BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Shine the light given the SQT transform.
	virtual void shine(const SQT&) const BOOST_NOEXCEPT_OR_NOTHROW = 0;

	virtual void set_brightness(const float brightness);
	float brightness() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Polymorphic output stream support for lights.
	friend std::ostream& operator << (std::ostream&, const light*);
	friend std::ostream& operator << (std::ostream&, const std::unique_ptr<light>&);
	friend std::ostream& operator << (std::ostream&, const std::shared_ptr<light>&);

	// Stream output support for a light.
	friend std::ostream& operator << (std::ostream&, const light&);

	// Needed because a light carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(intensity);
	}
};

/*****************************************************************************
* gintonic::ambient_light                                                    *
*****************************************************************************/

class ambient_light : public light
{
public:

	// Default constructor.
	ambient_light() = default;

	// Constructor that takes an intensity value.
	ambient_light(const vec4f& intensity);

	// Destructor.
	virtual ~ambient_light() BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Shine the ambient light. Note that an ambient
	// light doesn't do anything with the supplied SQT transform
	// at all, it just lights all objects uniformly.
	virtual void shine(const SQT&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Stream output support for a ambient light.
	friend std::ostream& operator << (std::ostream&, const ambient_light&);

	// Needed because a light carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(light);
	}
};

/*****************************************************************************
* gintonic::directional_light                                                *
*****************************************************************************/

class directional_light : public ambient_light
{
public:

	// Default constructor.
	directional_light() = default;
	
	// Constructor that takes an intensity value.
	directional_light(const vec4f& intensity);

	// Destructor.
	virtual ~directional_light() BOOST_NOEXCEPT_OR_NOTHROW;

	// Shine the directional light given the SQT transform.
	// In the case of a directional light, only the rotation
	// part of the SQT transform is used.
	virtual void shine(const SQT&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Stream output support for a directional light.
	friend std::ostream& operator << (std::ostream&, const directional_light&);

	// Needed because a light carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Serialization support.
	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ambient_light);
	}
};

/*****************************************************************************
* gintonic::point_light                                                      *
*****************************************************************************/

class point_light : public light
{
public:

	// Default constructor.
	point_light() = default;

	// Constructor that takes an intensity value.
	point_light(const vec4f& intensity);
	
	// Constructor that takes an intensity value and an attenuation value.
	point_light(const vec4f& intensity, const vec4f& attenuation);

	// Destructor.
	virtual ~point_light() BOOST_NOEXCEPT_OR_NOTHROW;

	// Obtain the attenuation value.
	vec4f attenuation() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Set the attenuation value. Setting a new attenuation value
	// also recalculates the cutoff radius (see below).
	void set_attenuation(const vec4f&) BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Obtain the cutoff radius as to where the light intensity
	// of the point light is so diminished that pixels do not
	// get any brighter.
	float cutoff_point() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Shine the point light given the SQT transform. The point light
	// uses only the translation (position) part of the SQT transform.
	virtual void shine(const SQT&) const BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void set_brightness(const float brightness);

	// Stream output support for a point light.
	friend std::ostream& operator << (std::ostream&, const point_light&);

	// Needed because a light carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	vec4f m_attenuation;

	float m_cutoff_point;

	void calculate_cutoff_radius() BOOST_NOEXCEPT_OR_NOTHROW;

	// Reimplement this method to support output streams.
	virtual std::ostream& pretty_print(std::ostream&) const BOOST_NOEXCEPT_OR_NOTHROW;

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

/*****************************************************************************
* gintonic::spot_light                                                       *
*****************************************************************************/

class spot_light : public point_light
{
public:

	// Default constructor.
	spot_light() = default;

	// Constructor that takes an intensity value.
	spot_light(const vec4f& intensity);
	
	// Constructor that takes an intensity value and an attenuation value.
	spot_light(const vec4f& intensity, const vec4f& attenuation);

	// Destructor.
	virtual ~spot_light() BOOST_NOEXCEPT_OR_NOTHROW;
	
	// Shine the spot light given the SQT transform. The spot light
	// uses both the rotation part as well as the translation (position)
	// part of the SQT transform. It does nothing with the scale part.
	virtual void shine(const SQT&) const BOOST_NOEXCEPT_OR_NOTHROW;

	// Stream output support for a spot light.
	friend std::ostream& operator << (std::ostream&, const spot_light&);

	// Needed because a light carries one or more vec4f's.
	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

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