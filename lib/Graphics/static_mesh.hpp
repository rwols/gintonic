/**
 * @file static_mesh.hpp
 * @brief Defines a static mesh class.
 * @author Raoul Wols
 */

#pragma once

#include "Mesh.hpp"

#include "../Math/mat3f.hpp"
#include "../Math/mat4f.hpp"

#include "OpenGL/VertexArrayObject.hpp"
#include "OpenGL/vector.hpp"

namespace FBX
{
	class FbxMesh; // Forward declaration.
}

namespace gintonic {

/**
 * @brief Static mesh component.
 */
class static_mesh : public Mesh
{
public:

	/// Default constructor.
	static_mesh();

	/**
	 * @brief Construct a static_mesh from an FbxMesh.
	 * 
	 * @param m A constant pointer to the FbxMesh.
	 * @param usagehint Specifies the usage hint to OpenGL. You probably want
	 * this to always be GL_STATIC_DRAW.
	 */
	static_mesh(const FBX::FbxMesh* m, 
		const GLenum usagehint = GL_STATIC_DRAW);

	/// Destructor.
	virtual ~static_mesh();

	virtual void draw() const noexcept final;

	virtual void draw(
		const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
		const std::vector<mat4f, allocator<mat4f>>& VM_matrices,
		const std::vector<mat3f, allocator<mat3f>>& N_matrices) final;

	/**
	 * @brief Set the static_mesh to the given FbxMesh.
	 * 
	 * @param m A constant pointer to the FbxMesh.
	 * @param usagehint Specifies the usage hint to OpenGL. You probably want
	 * this to always be GL_STATIC_DRAW.
	 */
	void set_data(const FBX::FbxMesh* m, 
		const GLenum usagehint = GL_STATIC_DRAW);

private:

	OpenGL::VertexArrayObject m_vao;

	// indices, positions, uvs, normals
	OpenGL::BufferObjectArray<4> m_buffer;
	
	// matrix_PVM, matrix_VM, matrix_N
	OpenGL::vector_array<GL_ARRAY_BUFFER, mat4f, 2> m_matrix_buffer;
	OpenGL::vector<GL_ARRAY_BUFFER, mat3f> m_matrix_N_buffer;
	
	GLsizei m_count = 0;
};

} // namespace gintonic
