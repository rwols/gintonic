#ifndef gintonic_camera_hpp
#define gintonic_camera_hpp

#include "vec3f.hpp"
#include "mat4f.hpp"

namespace gintonic {

class proj_info
{
public:

	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};

	projection_type projection;

	float fieldofview;

	mat4f matrix;

	void update() BOOST_NOEXCEPT_OR_NOTHROW;
};

class orthographic_info : public proj_info
{
private:

	float m_left = -1.0f;
	float m_right = 1.0f;
	float m_bottom = -1.0f;
	float m_top = 1.0f;

public:

	orthographic_info() = default;

	orthographic_info(
		const float left, 
		const float right, 
		const float bottom, 
		const float top, 
		const float near, 
		const float far);

	orthographic_info(
		const float width, 
		const float height, 
		const float near, 
		const float far);

	virtual ~orthographic_info();

	// Basic getters and setters.
	inline void set_left(const float value) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_left = value;
	}

	inline void set_right(const float value) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_right = value;
	}

	inline void set_bottom(const float value) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_bottom = value;
	}

	inline void set_top(const float value) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_top = value;
	}
	void set_width(const float value) BOOST_NOEXCEPT_OR_NOTHROW;
	void set_height(const float value) BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void update() BOOST_NOEXCEPT_OR_NOTHROW final;

};

class perspective_info : public proj_info
{
private:

	float m_fieldofview = 60.0f;
	float m_aspectratio = 1.6f;

public:

	perspective_info() = default;

	perspective_info(
		const float fieldofview, 
		const float aspectratio, 
		const float near, 
		const float far);

	virtual ~perspective_info();

	// Basic getters and setters.
	inline void set_fieldofview(const float value) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_fieldofview = value;
	}
	inline void set_aspectratio(const float value) BOOST_NOEXCEPT_OR_NOTHROW
	{
		m_aspectratio = value;
	}
	inline float fieldofview() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_fieldofview;
	}
	inline float aspectratio() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_aspectratio;
	}

	virtual void update() BOOST_NOEXCEPT_OR_NOTHROW final;
};

class camera
{
public:

	enum projection_type
	{
		kOrthographicProjection,
		kPerspectiveProjection
	};
	projection_type projection;

	float horizontal_angle;
	float vertical_angle;
	float fake_zoom;
	float orthographic_zoom;
	float field_of_view;
	float near_plane;
	float far_plane;
	
	vec3f position;
	vec3f direction;
	vec3f right;
	vec3f up;

	void add_horizontal_and_vertical_angles(const float horizontal, const float vertical) BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f get_matrix() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f matrix_V() const BOOST_NOEXCEPT_OR_NOTHROW;
	mat4f matrix_P(const int width, const int height) const BOOST_NOEXCEPT_OR_NOTHROW;
	void move_forward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_backward(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_left(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_right(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_up(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;
	void move_down(const float amount) BOOST_NOEXCEPT_OR_NOTHROW;


};

} // namespace gintonic

#endif