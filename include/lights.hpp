#ifndef gintonic_lights_hpp
#define gintonic_lights_hpp

#include "math.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

namespace gintonic {

class directional_light_pass_shader;
class point_light_pass_shader;

class light : std::enable_shared_from_this<light>
{
public:

	vec4f intensity;

	light() = default;

	light(const vec4f& intensity);
	
	virtual ~light() BOOST_NOEXCEPT_OR_NOTHROW;
	
	virtual void shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(intensity);
	}

};

class directional_light : public light
{
public:
	directional_light() = default;
	
	directional_light(const vec4f& intensity);

	virtual ~directional_light() BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	friend boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(light);
	}
};

class point_light : public light
{
public:

	point_light() = default;

	point_light(const vec4f& intensity);
	
	point_light(const vec4f& intensity, const vec3f& attenuation);

	virtual ~point_light() BOOST_NOEXCEPT_OR_NOTHROW;

	vec3f attenuation() const BOOST_NOEXCEPT_OR_NOTHROW;

	void set_attenuation(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	
	float cutoff_point() const BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW;

	GINTONIC_DEFINE_ALIGNED_OPERATOR_NEW_DELETE(16);

private:

	vec3f m_attenuation;

	float m_cutoff_point;

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
		vec3f att;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(light);
		ar & boost::serialization::make_nvp("attenuation", att);
		set_attenuation(att);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

} // namespace gintonic

#endif