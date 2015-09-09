/**
 * @file basic_shapes.hpp
 * @brief Defines several basic shapes that all inherit from mesh.
 * @author Raoul Wols
 */

#ifndef gintonic_basic_shapes_hpp
#define gintonic_basic_shapes_hpp

#include "mesh.hpp"
#include "opengl/buffer_object.hpp"
#include "opengl/buffer_object_array.hpp"
#include "opengl/vertex_array_object.hpp"

namespace gintonic {

union vec3f; // Forward declaration.

/**
 * @brief Unit quad with position information.
 * @details A quad that spans [-1,+1] x [-1,+1] in the XZ-plane. This quad
 * only carries position (P) information in its vertices.
 */
class unit_quad_P : public mesh
{
public:

	/// Default constructor.
	unit_quad_P();

	/// Destructor.
	virtual ~unit_quad_P();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;
	
private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

/**
 * @brief unit quad with positions and normals.
 * @details A quad that spans [-1,+1] x [-1,+1]. This quad carries position
 * (P) and normal (N) information, so it can be shaded by lights.
 */
class unit_quad_PN : public mesh
{
public:

	/// Default constructor.
	unit_quad_PN();

	/// Destructor.
	virtual ~unit_quad_PN();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

/**
 * @brief unit quad with positions and colors.
 * @details A quad that spans [-1,+1] x [-1,+1]. This quad carries position
 * (P) and color (C) information. The color values are fixed. They are
 * determined at construction time.
 */
class unit_quad_PC : public mesh
{
public:

	/**
	 * @brief Constructor that creates a unit_quad_PC with the given colors.
	 * 
	 * @param color_topleft The top-left color value.
	 * @param color_topright The top-right color value.
	 * @param color_bottomleft The bottom-left color value.
	 * @param color_bottomright The bottom-right color value.
	 */
	unit_quad_PC(const vec3f& color_topleft, const vec3f& color_topright, 
		const vec3f& color_bottomleft, const vec3f& color_bottomright);

	/// Destructor.
	virtual ~unit_quad_PC();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

/**
 * @brief unit quad with positions and texture coordinates.
 * @details A quad that spans [-1,+1] x [-1,+1]. This quad carries position
 * (P) and texture coordinates (U).
 */
class unit_quad_PU : public mesh
{
public:

	/// Default constructor.
	unit_quad_PU();

	/// Destructor.
	virtual ~unit_quad_PU();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};

/**
 * @brief Unit cube with position information.
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. This cube
 * carries only position information.
 */
class unit_cube_P : public mesh
{
public:

	/// Default constructor.
	unit_cube_P();

	/// Destructor.
	virtual ~unit_cube_P();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo, m_ibo;
};

/**
 * @brief Unit cube with position information. The triangles are flipped.
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. The triangles
 * are flipped (they are clockwise instead of counterclockwise). This means
 * that you can only see the cube when you are inside the cube, assuming
 * that culling is on and triangles are rendered counterclockwise.
 */
class unit_cube_P_flipped : public mesh
{
public:

	/// Default constructor.
	unit_cube_P_flipped();

	/// Destructor.
	virtual ~unit_cube_P_flipped();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo, m_ibo;
};

/**
 * @brief Unit cube with position and texture coordinates.
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. This cube
 * carries position (P) and texture coordinates (U).
 */
class unit_cube_PU : public mesh
{
public:

	/// Default constructor.
	unit_cube_PU();

	/// Destructor.
	virtual ~unit_cube_PU();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo, m_ibo;
};

/**
 * @brief Unit cube with positions, texture coordinates and normals.
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. This cube
 * carries positions (P), texture coordinates (U) and normals (N).
 */
class unit_cube_PUN : public mesh
{
public:

	/// Default constructor.
	unit_cube_PUN();

	/// Destructor.
	virtual ~unit_cube_PUN();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<3> m_buffer; // indices, positions, texcoords
};

/**
 * @brief Unit cube with positions, texture coordinates and normals.
 * The triangles are flipped.
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. The triangles
 * are flipped (they are clockwise instead of counterclockwise). This means
 * that you can only see the cube when you are inside the cube, assuming
 * that culling is on and triangles are rendered counterclockwise.
 */
class unit_cube_PUN_flipped_normals : public mesh
{
public:

	/// Default constructor.
	unit_cube_PUN_flipped_normals();

	/// Destructor.
	virtual ~unit_cube_PUN_flipped_normals();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<3> m_buffer; // indices, positions, texcoords
};

/**
 * @brief Unit cube with positions, texture coordinates, normals, tangents
 * and bitangents.
 * 
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. This cube
 * carries positions (P), texture coordinates (U), normals (N), tangents (T)
 * and bitangents (B).
 */
class unit_cube_PUNTB : public mesh
{
public:

	/// Default constructor.
	unit_cube_PUNTB();

	/// Destructor.
	virtual ~unit_cube_PUNTB();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	// indices, positions, texcoords, normals
	opengl::buffer_object_array<4> m_buffer;
};

/**
 * @brief Unit cube with positions, texture coordinates, normals, tangents
 * and bitangents. The triangles are flipped.
 * 
 * @details A unit cube that spans [-1,+1] x [-1,+1] x [-1,+1]. The triangles
 * are flipped (they are clockwise instead of counterclockwise). This means
 * that you can only see the cube when you are inside the cube, assuming
 * that culling is on and triangles are rendered counterclockwise.
 */
class unit_cube_PUNTB_flipped_normals : public mesh
{
public:

	/// Default constructor.
	unit_cube_PUNTB_flipped_normals();

	/// Destructor.
	virtual ~unit_cube_PUNTB_flipped_normals();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	// indices, positions, texcoords, normals
	opengl::buffer_object_array<4> m_buffer;
};

/**
 * @brief A unit sphere with positions.
 * @details The quality is determined by the stacks and slices parameter of
 * the constructor.
 */
class unit_sphere_P : public mesh
{
public:

	/**
	 * @brief Constructor.
	 * @details The quality is determined by the stacks and slices parameter
	 * of the constructor.
	 * 
	 * @param stacks The number of stacks.
	 * @param slices The number of slices.
	 */
	unit_sphere_P(const unsigned short stacks, const unsigned short slices);

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	/// Destructor.
	virtual ~unit_sphere_P();

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
	opengl::buffer_object m_ibo;
	unsigned short num_indices;
};

/**
 * @brief A unit sphere with positions and texture coordinates.
 * @details The quality is determined by the stacks and slices parameter of
 * the constructor.
 */
class unit_sphere_PU : public mesh
{
public:

	/**
	 * @brief Constructor.
	 * @details The quality is determined by the stacks and slices parameter
	 * of the constructor.
	 * 
	 * @param stacks The number of stacks.
	 * @param slices The number of slices.
	 */
	unit_sphere_PU(const unsigned short stacks, const unsigned short slices);
	
	/// Destructor.
	virtual ~unit_sphere_PU();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
	opengl::buffer_object m_ibo;
	unsigned short num_indices;
};

/**
 * @brief A unit sphere with positions, texture coordinates and normals.
 * @details The quality is determined by the subdivisions parameter of the
 * constructor.
 */
class unit_sphere_PUN : public mesh
{
public:

	/**
	 * @brief Constructor.
	 * @details The quality is determined by the subdivisions parameter of
	 * the constructor. The way this sphere is made is to begin with a unit
	 * cube, and then applying the continuous map that sends a vector to
	 * itself divided by its length. This gives you essentially six
	 * deformed patches that smoothly attach together. The six patches come
	 * from the six sides of the cube. We don't use the "stacks and slices"
	 * method to construct this sphere because this method works a lot better
	 * with texture coordinates.
	 * 
	 * @param subdivisions The number of subdivisions of the sphere.
	 */
	unit_sphere_PUN(const unsigned short subdivisions);

	/// Destructor.
	virtual ~unit_sphere_PUN();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object_array<3> m_buffer; // indices, positions, texcoords
	GLsizei m_count;
};

/**
 * @brief A unit cone with positions.
 * @details The base of the cone is -1 floats under the XZ-plane. The top of
 * the cone is +1 on the Y-axis. This probably needs to change: it seems
 * more natural to let the cone point in the default forward direction
 * [0,0,-1], it now points in the direction [0,1,0] by default. You need to
 * apply a rotation matrix that rotates 90 degrees around the X-axis to get
 * the cone point in the default [0,0,-1] direction.
 */
class unit_cone_P : public mesh
{
public:

	/**
	 * @brief Constructor.
	 * @details The quality of the base, and hence of the cone, is determined
	 * by the number of divivisions.
	 * 
	 * @param divisions The number of divisions.
	 */
	unit_cone_P(const GLsizei divisions);

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	/// Destructor.
	virtual ~unit_cone_P();

	/// The number of divisions. This cannot be changed.
	const GLsizei divisions;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vertices;
};

/**
 * @brief Unit cone with positions and normals.
 * @details The base of the cone is -1 floats under the XZ-plane. The top of
 * the cone is +1 on the Y-axis. This probably needs to change: it seems
 * more natural to let the cone point in the default forward direction
 * [0,0,-1], it now points in the direction [0,1,0] by default. You need to
 * apply a rotation matrix that rotates 90 degrees around the X-axis to get
 * the cone point in the default [0,0,-1] direction.
 */
class unit_cone_PN : public mesh
{
public:

	/**
	 * @brief Constructor.
	 * @details The quality of the base, and hence of the cone, is determined
	 * by the number of divivisions.
	 * 
	 * @param divisions The number of divisions.
	 */
	unit_cone_PN(const GLsizei divisions);

	/// Destructor.
	virtual ~unit_cone_PN();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	/// The number of divisions. This cannot be changed.
	const GLsizei divisions;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vbo;
};


/**
 * @brief Unit cylinder with positions.
 * The base of the cylinder is -1 floats under the XZ-plane. The top of the
 * cylinder is +1 above the XZ-plane.
 */
class unit_cylinder_P : public mesh
{
public:

	/**
	 * @brief Constructor.
	 * @details The quality of the circle of the top and bottom of the
	 * cylinder is determined by the number of divisions.
	 * 
	 * @param divisions The number of divisions.
	 */
	unit_cylinder_P(const GLsizei divisions);

	/// Destructor.
	virtual ~unit_cylinder_P();

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

	/// The number of divisions. This cannot be changed.
	const GLsizei divisions;

private:
	opengl::vertex_array_object m_vao;
	opengl::buffer_object m_vertices;
};

} // end of namespace gintonic

#endif