/**
 * @file static_mesh.hpp
 * @author Raoul Wols
 */

#ifndef gintonic_static_mesh_hpp
#define gintonic_static_mesh_hpp

#include "mesh.hpp"
#include "mat3f.hpp"
#include "mat4f.hpp"
#include "opengl/vertex_array_object.hpp"
#include "opengl/vector.hpp"

namespace FBX
{
	class FbxMesh; // Forward declaration.
}

namespace gintonic {

/**
 * @brief Static mesh component.
 */
class static_mesh : public mesh
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

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW final;

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

	opengl::vertex_array_object m_vao;

	// indices, positions, uvs, normals
	opengl::buffer_object_array<4> m_buffer;
	
	// matrix_PVM, matrix_VM, matrix_N
	opengl::vector_array<GL_ARRAY_BUFFER, mat4f, 2> m_matrix_buffer;
	opengl::vector<GL_ARRAY_BUFFER, mat3f> m_matrix_N_buffer;
	
	GLsizei m_count = 0;
};

} // namespace gintonic

#endif