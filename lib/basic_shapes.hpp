#ifndef gintonic_basic_shapes_hpp
#define gintonic_basic_shapes_hpp

#include "mesh.hpp"
#include "opengl/buffer_object.hpp"
#include "opengl/buffer_object_array.hpp"
#include "opengl/vertex_array_object.hpp"

namespace gintonic {

union vec3f; // Forward declaration.

/*****************************************************************************
* gintonic::unit_quad_P                                                      *
*                                                                            *
* Purpose: A quad that spans [-1,+1] x [-1,+1]. This quad only carries       *
*          position (P) information in its vertices.                         *
*****************************************************************************/
class unit_quad_P : public mesh
{
public:

	// Default constructor.
	unit_quad_P();

	// Destructor.
	virtual ~unit_quad_P();

	// Draw the quad.
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
	
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

/*****************************************************************************
* gintonic::unit_quad_PN                                                     *
*                                                                            *
* Purpose: A quad that spans [-1,+1] x [-1,+1]. This quad carries position   *
*          (P) and normal (N) information, so it can be shaded by lights.    *
*****************************************************************************/
class unit_quad_PN : public mesh
{
public:

	// Default constructor.
	unit_quad_PN();

	// Destructor.
	virtual ~unit_quad_PN();

	// Draw the quad.
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

class unit_quad_PC : public mesh
{
public:
	unit_quad_PC(const vec3f& color_topleft, const vec3f& color_topright, 
		const vec3f& color_bottomleft, const vec3f& color_bottomright);
	virtual ~unit_quad_PC();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

class unit_quad_PU : public mesh
{
public:
	unit_quad_PU();
	virtual ~unit_quad_PU();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

class unit_cube_P : public mesh
{
public:
	unit_cube_P();
	virtual ~unit_cube_P();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo, m_ibo;
};

class unit_cube_P_flipped : public mesh
{
public:
	unit_cube_P_flipped();
	virtual ~unit_cube_P_flipped();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo, m_ibo;
};

class unit_cube_PU : public mesh
{
public:
	unit_cube_PU();
	virtual ~unit_cube_PU();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo, m_ibo;
};

class unit_cube_PUN : public mesh
{
public:
	unit_cube_PUN();
	virtual ~unit_cube_PUN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<3> m_buffer; // indices, positions, texcoords
};

class unit_cube_PUN_flipped_normals : public mesh
{
public:
	unit_cube_PUN_flipped_normals();
	virtual ~unit_cube_PUN_flipped_normals();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<3> m_buffer; // indices, positions, texcoords
};

class unit_cube_PUNTB : public mesh
{
public:
	unit_cube_PUNTB();
	virtual ~unit_cube_PUNTB();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<4> m_buffer; // indices, positions, texcoords, normals
};

class unit_cube_PUNTB_flipped_normals : public mesh
{
public:
	unit_cube_PUNTB_flipped_normals();
	virtual ~unit_cube_PUNTB_flipped_normals();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<4> m_buffer; // indices, positions, texcoords, normals
};

class unit_sphere_P : public mesh
{
public:
	unit_sphere_P(const unsigned short stacks, const unsigned short slices);
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
	virtual ~unit_sphere_P();
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
	opengl::buffer_object m_ibo;
	unsigned short num_indices;
};

class unit_sphere_PU : public mesh
{
public:
	unit_sphere_PU(const unsigned short stacks, const unsigned short slices);
	virtual ~unit_sphere_PU();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
	opengl::buffer_object m_ibo;
	unsigned short num_indices;
};

class unit_sphere_PUN : public mesh
{
public:
	unit_sphere_PUN(const unsigned short subdivisions);
	virtual ~unit_sphere_PUN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<3> m_buffer; // indices, positions, texcoords
	GLsizei m_count;
};

class unit_cone_P : public mesh
{
public:
	unit_cone_P(const GLsizei divisions);
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
	virtual ~unit_cone_P();
	const GLsizei divisions;
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vertices;
};

class unit_cone_PN : public mesh
{
public:
	unit_cone_PN(const GLsizei divisions);
	virtual ~unit_cone_PN();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	const GLsizei divisions;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

class unit_cylinder_P : public mesh
{
public:
	unit_cylinder_P(const GLsizei divisions);
	virtual ~unit_cylinder_P();
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	const GLsizei divisions;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vertices;
};

} // end of namespace gintonic

#endif