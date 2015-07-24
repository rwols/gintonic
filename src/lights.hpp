#ifndef gintonic_lights_hpp
#define gintonic_lights_hpp

#include "math.hpp"

namespace gintonic {

class directional_light_pass_shader;
class point_light_pass_shader;

class light : std::enable_shared_from_this<light>
{
protected:
	light() = default;
	light(const vec3f& intensity);
public:
	virtual ~light() BOOST_NOEXCEPT_OR_NOTHROW = default;
	virtual void shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW = 0;
	vec3f intensity;
};

class directional_light : public light
{
public:
	directional_light(
		std::shared_ptr<directional_light_pass_shader> prog);
	
	directional_light(
		std::shared_ptr<directional_light_pass_shader> prog,
		const vec3f& intensity);

	virtual ~directional_light() BOOST_NOEXCEPT_OR_NOTHROW = default;
	virtual void shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW;
private:
	std::shared_ptr<directional_light_pass_shader> m_program;
};

class point_light : public light
{
public:

	point_light(
		std::shared_ptr<point_light_pass_shader> prog);

	point_light(
		std::shared_ptr<point_light_pass_shader> prog,
		const vec3f& intensity);
	
	point_light(
		std::shared_ptr<point_light_pass_shader> prog,
		const vec3f& intensity,
		const vec3f& attenuation);

	virtual ~point_light() BOOST_NOEXCEPT_OR_NOTHROW = default;

	vec3f attenuation() const BOOST_NOEXCEPT_OR_NOTHROW;
	void set_attenuation(const vec3f&) BOOST_NOEXCEPT_OR_NOTHROW;
	float cutoff_point() const BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void shine(const sqt_transformf&) const BOOST_NOEXCEPT_OR_NOTHROW;

private:
	std::shared_ptr<point_light_pass_shader> m_program;
	vec3f m_attenuation;
	float m_cutoff_point;
};

} // namespace gintonic

#endif