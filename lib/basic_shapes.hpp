/*!
\file basic_shapes.hpp
\author Raoul Wols
\date 17/09/2014
\brief Quad, cube, sphere, cone and cylinder.
*/

#ifndef basic_shapes_hpp
#define basic_shapes_hpp

#include "opengl.hpp"

namespace gintonic {
namespace opengl {

/*!
\brief The base class for other shapes.

It has a pure virtual method basic_shape::draw(), which
will draw the basic shape. That is, it will bind the correct
vertex buffers, bind the correct vertex array object, and make
a draw call to OpenGL.

\sa unit_quad_P
\sa unit_sphere_P
\sa unit_cone_P
*/
class basic_shape
{
public:
	virtual ~basic_shape() = default;
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW = 0;
};

/*!
\brief Simple unit quad (a rectangle), where its vertices only have position (P) information.

The coordinates of the quad are (-1,-1,0), (1,-1,0), (1,1,0) and (-1,1,0). If you need something
smaller, just set an appropriate model matrix in your pipeline.

\sa basic_shape
\sa unit_sphere_P
\sa unit_cone_P
*/
class unit_quad_P : public basic_shape
{
public:
	unit_quad_P();
	virtual ~unit_quad_P() = default;
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_quad_PN : public basic_shape
{
public:
	unit_quad_PN();
	virtual ~unit_quad_PN() = default;
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

class unit_quad_PC : public basic_shape
{
public:

	/*!
	\brief Contructs a unit_quad_PC.
	\param [in] color_topleft The color value for the top left corner.
	\param [in] color_topright The color value for the top right corner.
	\param [in] color_bottomleft The color value for the bottom left corner.
	\param [in] color_bottomright The color value for the bottom right corner.
	*/
	unit_quad_PC(const vec3f& color_topleft, const vec3f& color_topright, 
		const vec3f& color_bottomleft, const vec3f& color_bottomright);

	/*!
	\brief Destroys a unit_quad_PC.
	*/
	virtual ~unit_quad_PC();

	/*!
	\brief Draws a unit_quad_P.
	*/
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

/*!
\brief Simple unit sphere, where its vertices only have position (P) information.

The sphere is divided up into stacks and slices. Think of this like peeling an
orange in one large slice, from top to bottom. The stacks are the "vertical"
cuts, while the slices are the "horizontal" cuts. You must give these two
parameters to the constructor. In practise, 64 stacks and 64 slices makes a nice
round sphere.

\sa basic_shape
\sa unit_quad_P
\sa unit_cone_P
*/
class unit_sphere_P : public basic_shape
{
public:

	/*!
	\brief Contructs a unit_sphere_P.

	The sphere is divided up into stacks and slices. Think of this like peeling an
	orange in one large slice, from top to bottom. The stacks are the "vertical"
	cuts, while the slices are the "horizontal" cuts. You must give these two
	parameters to the constructor. In practise, 64 stacks and 64 slices makes a nice
	round sphere.

	\param [in] stacks The number of stacks. That is, the number of "vertical" cuts.
	\param [in] slices The number of slices. That is, the number of "horizontal" cuts.
	*/
	unit_sphere_P(const unsigned short stacks, const unsigned short slices);

	/*!
	\brief Destroys a unit_sphere_P.
	*/
	virtual ~unit_sphere_P();

	/*!
	\brief Draws a unit_sphere_P.
	*/
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
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
	// buffer_object m_ibo;
	GLsizei m_count;
};

/*!
\brief Simple unit cone, where its vertices only have position (P) information.

The bottom circle of the cone is approximated with triangles. The higher the number
of divisions, the better. Essentially, the number of divisions is the number of triangles
arranged in a fan-like pattern at the bottom circle of the cone.

\sa basic_shape
\sa unit_quad_P
\sa unit_sphere_P
*/
class unit_cone_P : public basic_shape
{
public:

	/*!
	\brief Contructs a unit_cone_P.

	The bottom circle of the cone is approximated with triangles. The higher the number
	of divisions, the better. Essentially, the number of divisions is the number of triangles
	arranged in a fan-like pattern at the bottom circle of the cone.

	\param [in] divisions The number of divisions.
	*/
	unit_cone_P(const GLsizei divisions);

	/*!
	\brief Destroys a unit_cone_P.
	*/
	virtual ~unit_cone_P();

	/*!
	\brief Draws a unit_cone_P
	*/
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	const GLsizei divisions; //!< The number of divisions of this cone.

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

	const GLsizei divisions; //!< The number of divisions of this cone.
private:
	vertex_array_object m_vao;
	buffer_object m_vbo;
};

/*!
\brief Simple unit cylinder, where its vertices only have position (P) information.

The circles of the cylinder are approximated with triangles. The higher the number
of divisions, the better. Essentially, the number of divisions is the number of triangles
arranged in a fan-like pattern at the bottom and top circle of the cylinder.

\sa basic_shape
\sa unit_quad_P
\sa unit_sphere_P
*/
class unit_cylinder_P : public basic_shape
{
public:

	/*!
	\brief Constructs a unit_cylinder_P.

	The circle of the cylinder is approximated with triangles. The higher the number
	of divisions, the better the approximation. The number of divisions is the number
	of triangles arranged in a fan-like pattern.

	\param [in] division The number of divisions.
	*/
	unit_cylinder_P(const GLsizei divisions);

	/*!
	\brief Destroys a unit_cylinder_P.
	*/
	virtual ~unit_cylinder_P();

	/*!
	\brief Draws a unit_cylinder_P
	*/
	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	const GLsizei divisions; //!< The number of divisions of this cylinder.

private:
	vertex_array_object m_vao;
	buffer_object m_vertices;
};

} // end of namespace opengl
} // end of namespace gintonic

#endif