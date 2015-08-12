#ifndef basic_shapes_hpp
#define basic_shapes_hpp

#include "opengl.hpp"

namespace gintonic {
namespace opengl {

class basic_shape
{
public:
	virtual ~basic_shape();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW = 0;
};

class unit_quad_P : public basic_shape
{
public:
	unit_quad_P();
	virtual ~unit_quad_P();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_quad_PN : public basic_shape
{
public:
	unit_quad_PN();
	virtual ~unit_quad_PN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_quad_PC : public basic_shape
{
public:
	unit_quad_PC(const vec3f& color_topleft, const vec3f& color_topright, 
		const vec3f& color_bottomleft, const vec3f& color_bottomright);
	virtual ~unit_quad_PC();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_quad_PU : public basic_shape
{
public:
	unit_quad_PU();
	virtual ~unit_quad_PU();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_cube_P : public basic_shape
{
public:
	unit_cube_P();
	virtual ~unit_cube_P();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo, m_ibo;
};

class unit_cube_PU : public basic_shape
{
public:
	unit_cube_PU();
	virtual ~unit_cube_PU();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo, m_ibo;
};

class unit_cube_PUN : public basic_shape
{
public:
	unit_cube_PUN();
	virtual ~unit_cube_PUN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo, m_ibo;
};

class unit_cube_PUNTB : public basic_shape
{
public:
	unit_cube_PUNTB();
	virtual ~unit_cube_PUNTB();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
	buffer_object m_ibo;
};

class unit_sphere_P : public basic_shape
{
public:
	unit_sphere_P(const unsigned short stacks, const unsigned short slices);
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
	virtual ~unit_sphere_P();
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
	buffer_object m_ibo;
	unsigned short num_indices;
};

class unit_sphere_PU : public basic_shape
{
public:
	unit_sphere_PU(const unsigned short stacks, const unsigned short slices);
	virtual ~unit_sphere_PU();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
	buffer_object m_ibo;
	unsigned short num_indices;
};

class unit_sphere_PUN : public basic_shape
{
public:
	unit_sphere_PUN(const unsigned short subdivisions);
	virtual ~unit_sphere_PUN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
	GLsizei m_count;
};

class unit_cone_P : public basic_shape
{
public:
	unit_cone_P(const GLsizei divisions);
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
	virtual ~unit_cone_P();
	const GLsizei divisions;
private:
	vertex_array_object m_vao;
	buffer_object m_vertices;
};

class unit_cone_PN : public basic_shape
{
public:
	unit_cone_PN(const GLsizei divisions);
	virtual ~unit_cone_PN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	const GLsizei divisions;

private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_cylinder_P : public basic_shape
{
public:
	unit_cylinder_P(const GLsizei divisions);
	virtual ~unit_cylinder_P();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	const GLsizei divisions;

private:
	vertex_array_object m_vao;
	buffer_object m_vertices;
};

} // end of namespace opengl
} // end of namespace gintonic

#endif