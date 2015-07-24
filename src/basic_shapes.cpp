#include "basic_shapes.hpp"
#include "vertices.hpp"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif

namespace gintonic {
namespace opengl {

unit_quad_P::unit_quad_P() : basic_shape()
{
	#ifdef BOOST_MSVC
	const int num_vertices = 4;
	static const vertex_P<GLfloat> quad[4] =
	#else
	BOOST_CONSTEXPR int num_vertices = 4;
	static const vertex_P<GLfloat> quad[num_vertices] =
	#endif
	{
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f},
		{-1.0f,  1.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f}
	};
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	vertex_P<GLfloat>::enable_attributes();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_P<GLfloat>) * num_vertices, quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void unit_quad_P::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_quad_PN::unit_quad_PN()
{
	const vertex_PN<GLfloat> quad[4] =
	{
		// --positions-----|---normals--------
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{ 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{ 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f}
	};
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	vertex_PN<GLfloat>::enable_attributes();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_PN<GLfloat>) * 4, quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void unit_quad_PN::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_quad_PC::unit_quad_PC(
const vec3f& color_topleft, const vec3f& color_topright,
const vec3f& color_bottomleft, const vec3f& color_bottomright)
{
	std::vector<vertex_PC<GLfloat>> quad(4);
	quad[0].position[0] = -1.0f;
	quad[0].position[1] = -1.0f;
	quad[0].position[2] =  0.0f;
	quad[0].color[0] = color_bottomleft[0];
	quad[0].color[1] = color_bottomleft[1];
	quad[0].color[2] = color_bottomleft[2];
	quad[0].color[3] = 1.0f;
	quad[0].position[0] =  1.0f;
	quad[0].position[1] = -1.0f;
	quad[0].position[2] =  0.0f;
	quad[0].color[0] = color_bottomright[0];
	quad[0].color[1] = color_bottomright[1];
	quad[0].color[2] = color_bottomright[2];
	quad[0].color[3] = 1.0f;
	quad[0].position[0] = -1.0f;
	quad[0].position[1] =  1.0f;
	quad[0].position[2] =  0.0f;
	quad[0].color[0] = color_topleft[0];
	quad[0].color[1] = color_topleft[1];
	quad[0].color[2] = color_topleft[2];
	quad[0].color[3] = 1.0f;
	quad[0].position[0] = 1.0f;
	quad[0].position[1] = 1.0f;
	quad[0].position[2] = 0.0f;
	quad[0].color[0] = color_topright[0];
	quad[0].color[1] = color_topright[1];
	quad[0].color[2] = color_topright[2];
	quad[0].color[3] = 1.0f;
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	vertex_PC<GLfloat>::enable_attributes();
	gtBufferData(GL_ARRAY_BUFFER, quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_quad_PC::~unit_quad_PC() {}

void unit_quad_PC::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_quad_PU::unit_quad_PU()
{
	#ifdef BOOST_MSVC
	const int num_vertices = 4;
	static const vertex_PU<GLfloat> quad[4] =
	#else
	BOOST_CONSTEXPR int num_vertices = 4;
	static const vertex_PU<GLfloat> quad[num_vertices] =
	#endif
	{
		{-1.0f, -1.0f, 0.0f, -1.0f, -1.0f},
		{ 1.0f, -1.0f, 0.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f, 0.0f, -1.0f,  1.0f},
		{ 1.0f,  1.0f, 0.0f,  1.0f,  1.0f}
	};
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	vertex_PU<GLfloat>::enable_attributes();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_PU<GLfloat>) * num_vertices, quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_quad_PU::~unit_quad_PU() {}

void unit_quad_PU::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define NUM_CUBE_INDICES 14
#define NUM_CUBE_VERTICES 8

unit_cube_P::unit_cube_P() : basic_shape()
{
	const vertex_P<GLfloat> cube_vertices[NUM_CUBE_VERTICES] =
	{
		{ 1.0f,  1.0f, -1.0f}, // 0
		{-1.0f,  1.0f, -1.0f}, // 1
		{ 1.0f, -1.0f, -1.0f}, // 2
		{-1.0f, -1.0f, -1.0f}, // 3
		{ 1.0f,  1.0f,  1.0f}, // 4
		{-1.0f,  1.0f,  1.0f}, // 5
		{-1.0f, -1.0f,  1.0f}, // 6
		{ 1.0f, -1.0f,  1.0f}, // 7
	};
	BOOST_CONSTEXPR GLubyte cube_indices[NUM_CUBE_INDICES] = {3,2,6,7,4,2,0,3,1,6,5,4,1,0};
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	vertex_P<GLfloat>::enable_attributes();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_P<GLfloat>) * NUM_CUBE_VERTICES, cube_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * NUM_CUBE_INDICES, cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cube_P::~unit_cube_P()
{

}

void unit_cube_P::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawElements(GL_TRIANGLE_STRIP, NUM_CUBE_INDICES, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cube_PU::unit_cube_PU() : basic_shape()
{
	const vertex_PU<GLfloat> cube_vertices[24] =
	{
		// front
    	{-1.0f, -1.0f,  1.0f, 0.0f, 0.0f},
    	{ 1.0f, -1.0f,  1.0f, 1.0f, 0.0f},
    	{ 1.0f,  1.0f,  1.0f, 1.0f, 1.0f},
    	{-1.0f,  1.0f,  1.0f, 0.0f, 1.0f},
    	// top
    	{-1.0f,  1.0f,  1.0f, 0.0f, 0.0f},
    	{ 1.0f,  1.0f,  1.0f, 1.0f, 0.0f},
    	{ 1.0f,  1.0f, -1.0f, 1.0f, 1.0f},
    	{-1.0f,  1.0f, -1.0f, 0.0f, 1.0f},
    	// back
    	{ 1.0f, -1.0f, -1.0f, 0.0f, 0.0f},
    	{-1.0f, -1.0f, -1.0f, 1.0f, 0.0f},
    	{-1.0f,  1.0f, -1.0f, 1.0f, 1.0f},
    	{ 1.0f,  1.0f, -1.0f, 0.0f, 1.0f},
    	// bottom
    	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f},
    	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f},
    	{ 1.0f, -1.0f,  1.0f, 1.0f, 1.0f},
    	{-1.0f, -1.0f,  1.0f, 0.0f, 1.0f},
    	// left
    	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f},
    	{-1.0f, -1.0f,  1.0f, 1.0f, 0.0f},
    	{-1.0f,  1.0f,  1.0f, 1.0f, 1.0f},
    	{-1.0f,  1.0f, -1.0f, 0.0f, 1.0f},
    	// right
    	{ 1.0f, -1.0f,  1.0f, 0.0f, 0.0f},
    	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f},
    	{ 1.0f,  1.0f, -1.0f, 1.0f, 1.0f},
    	{ 1.0f,  1.0f,  1.0f, 0.0f, 1.0f}
	};
	BOOST_CONSTEXPR GLubyte cube_indices[36] =
	{
		// front
     	0,  1,  2,
     	2,  3,  0,
    	// top
    	4,  5,  6,
    	6,  7,  4,
    	// back
    	8,  9, 10,
    	10, 11,  8,
    	// bottom
    	12, 13, 14,
    	14, 15, 12,
    	// left
    	16, 17, 18,
    	18, 19, 16,
    	// right
    	20, 21, 22,
    	22, 23, 20
	};
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	vertex_PU<GLfloat>::enable_attributes();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_PU<GLfloat>) * 24, cube_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cube_PU::~unit_cube_PU()
{

}

void unit_cube_PU::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cube_PUN::unit_cube_PUN()
{
	const vertex_PUN<GLfloat> cube_vertices[24] =
	{
		// front
		// --positions------|---uvs-----|---normals-----------
    	{-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f},
    	{ 1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f},
    	{ 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f},
    	{-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f},
    	// top
    	// --positions------|---uvs-----|---normals-----------
    	{-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f},
    	{ 1.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f},
    	{ 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f},
    	{-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f},
    	// back
    	// --positions------|---uvs-----|---normals-----------
    	{ 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f},
    	{-1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f},
    	{-1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f},
    	{ 1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f},
    	// bottom
    	// --positions------|---uvs-----|---normals-----------
    	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f},
    	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f},
    	{ 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f},
    	{-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f},
    	// left
    	// --positions------|---uvs-----|---normals-----------
    	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f},
    	{-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f},
    	{-1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f},
    	{-1.0f,  1.0f, -1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f},
    	// right
    	// --positions------|---uvs-----|---normals-----------
    	{ 1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f},
    	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f},
    	{ 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f},
    	{ 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f},
	};
	BOOST_CONSTEXPR GLubyte cube_indices[36] =
	{
		// front
     	0,  1,  2,
     	2,  3,  0,
    	// top
    	4,  5,  6,
    	6,  7,  4,
    	// back
    	8,  9, 10,
    	10, 11,  8,
    	// bottom
    	12, 13, 14,
    	14, 15, 12,
    	// left
    	16, 17, 18,
    	18, 19, 16,
    	// right
    	20, 21, 22,
    	22, 23, 20
	};
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	vertex_PUN<GLfloat>::enable_attributes();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_PUN<GLfloat>) * 24, cube_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cube_PUN::~unit_cube_PUN()
{

}

void unit_cube_PUN::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_sphere_P::unit_sphere_P(const unsigned short stacks, const unsigned short slices) 
{
	std::vector<vertex_P<GLfloat>> vertices;
	std::vector<unsigned short> indices;
	float theta, phi, sin_theta, sin_phi, cos_theta, cos_phi;

	for (unsigned int stack = 0; stack <= stacks; ++stack)
	{
		for (unsigned int slice = 0; slice < slices; ++slice)
		{
			theta = stack * static_cast<float>(M_PI) / stacks;
			phi = slice * 2.0f * static_cast<float>(M_PI) / slices;
			sin_theta = std::sin(theta);
			sin_phi = std::sin(phi);
			cos_theta = std::cos(theta);
			cos_phi = std::cos(phi);
			vertices.emplace_back(vec3f(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta));
		}
	}
	for (unsigned int stack = 0; stack < stacks; ++stack)
	{
		for (unsigned int slice = 0; slice <= slices; ++slice)
		{
			indices.push_back((stack * slices) + (slice % slices));
			indices.push_back(((stack + 1) * slices) + (slice % slices));
		}
	}
	// number of indices: stacks * slices * 2
	num_indices = static_cast<unsigned short>(indices.size());

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	vertex_P<GLfloat>::enable_attributes();
	gtBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
}

void unit_sphere_P::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_sphere_P::~unit_sphere_P()
{

}

unit_sphere_PU::unit_sphere_PU(const unsigned short stacks, const unsigned short slices)
{
	std::vector<vertex_PU<GLfloat>> vertices;
	std::vector<unsigned short> indices;
	float theta, phi, sin_theta, sin_phi, cos_theta, cos_phi;

	for (unsigned int stack = 0; stack <= stacks; ++stack)
	{
		for (unsigned int slice = 0; slice < slices; ++slice)
		{
			theta = stack * static_cast<float>(M_PI) / stacks;
			phi = slice * 2.0f * static_cast<float>(M_PI) / slices;
			sin_theta = std::sin(theta);
			sin_phi = std::sin(phi);
			cos_theta = std::cos(theta);
			cos_phi = std::cos(phi);
			vertices.emplace_back
			(
				vec3f(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta),
				vec2f(0, 0) // TODO: THIS IS A PLACEHOLDER!!!
			);
		}
	}
	for (unsigned int stack = 0; stack < stacks; ++stack)
	{
		for (unsigned int slice = 0; slice <= slices; ++slice)
		{
			indices.push_back((stack * slices) + (slice % slices));
			indices.push_back(((stack + 1) * slices) + (slice % slices));
		}
	}
	// number of indices: stacks * slices * 2
	num_indices = static_cast<unsigned short>(indices.size());

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	vertex_PU<GLfloat>::enable_attributes();
	gtBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
}

unit_sphere_PU::~unit_sphere_PU()
{

}

void unit_sphere_PU::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_sphere_PUN::unit_sphere_PUN(const unsigned short stacks, const unsigned short slices)
{

}

unit_sphere_PUN::~unit_sphere_PUN()
{

}

void unit_sphere_PUN::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cone_P::unit_cone_P(const GLsizei divs) : basic_shape(), divisions(divs+2)
{
	std::vector<vertex_P<GLfloat>> vertices(2 * divisions);
	GLfloat s, c;
	const GLfloat angle = 2.0f * static_cast<GLfloat>(M_PI) / static_cast<GLfloat>(divs);

	vertices[0].position[0] = 0;
	vertices[0].position[1] = -1;
	vertices[0].position[2] = 0;
	vertices[divisions].position[0] = 0;
	vertices[divisions].position[1] = 1;
	vertices[divisions].position[2] = 0;
	for (GLsizei div = 1; div < divisions; ++div)
	{
		c = cos(static_cast<GLfloat>(div-1) * angle);
		s = sin(static_cast<GLfloat>(div-1) * angle);
		vertices[div].position[0] = c;
		vertices[div].position[1] = -1.0f;
		vertices[div].position[2] = s;
		vertices[divisions+div].position[0] = c;
		vertices[divisions+div].position[1] = -1.0f;
		vertices[divisions+div].position[2] = s;
	}
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	vertex_P<GLfloat>::enable_attributes();
	gtBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cone_P::~unit_cone_P()
{

}

void unit_cone_P::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, divisions);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLE_FAN, divisions, divisions);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glFrontFace(GL_CCW);
}

unit_cylinder_P::unit_cylinder_P(const GLsizei divs) : basic_shape(), divisions(divs+2)
{
	std::vector<vertex_P<GLfloat>> vertices(4 * divisions);
	GLfloat s_current, c_current, s_next, c_next;
	const GLfloat angle = 2.0f * static_cast<GLfloat>(M_PI) / static_cast<GLfloat>(divs);

	c_current = 1.0f; // cos(0) = 1
	s_current = 0.0f; // sin(0) = 0
	for (GLsizei div = 0; div < divisions; ++div)
	{
		// c_current = cos(static_cast<GLfloat>(div)   * angle);
		// s_current = sin(static_cast<GLfloat>(div)   * angle);
		c_next    = cos(static_cast<GLfloat>(div+1) * angle);
		s_next    = sin(static_cast<GLfloat>(div+1) * angle);

		vertices[div].position[0] = c_current;
		vertices[div].position[1] = -1.0f;
		vertices[div].position[2] = s_current;

		vertices[divisions+div].position[0] = c_current;
		vertices[divisions+div].position[1] = 1.0f;
		vertices[divisions+div].position[2] = s_current;

		vertices[2*divisions+2*div+0].position[0] = vertices[divisions+div].position[0];
		vertices[2*divisions+2*div+0].position[1] = vertices[divisions+div].position[1];
		vertices[2*divisions+2*div+0].position[2] = vertices[divisions+div].position[2];
		vertices[2*divisions+2*div+1].position[0] = vertices[div].position[0];
		vertices[2*divisions+2*div+1].position[1] = vertices[div].position[1];
		vertices[2*divisions+2*div+1].position[2] = vertices[div].position[2];

		c_current = c_next;
		s_current = s_next;
	}
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	vertex_P<GLfloat>::enable_attributes();
	gtBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unit_cylinder_P::~unit_cylinder_P()
{

}

void unit_cylinder_P::draw() const BOOST_NOEXCEPT_OR_NOTHROW
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, divisions);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLE_FAN, divisions, divisions);
	glDrawArrays(GL_TRIANGLE_STRIP, 2*divisions, 2*divisions);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glFrontFace(GL_CCW);
}

} // end of namespace opengl
} // end of namespace gintonic