#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "vertices.hpp"
#include "vector.hpp"

namespace gintonic {
	
	class mesh : public std::enable_shared_from_this<mesh>
	{
	public:

		mesh();
		mesh(GINTONIC_NAMESPACE_FBX::FbxMesh*, const GLenum usagehint = GL_STATIC_DRAW);

		void draw() const BOOST_NOEXCEPT_OR_NOTHROW;

		void draw(
			const std::vector<mat4f>& PVM_matrices_transposed,
			const std::vector<mat4f>& VM_matrices_transposed,
			const std::vector<mat3f>& N_matrices_transposed);

		struct vec2f
		{
			GLfloat x;
			GLfloat y;

			vec2f() = default;
			vec2f(const GINTONIC_NAMESPACE_FBX::FbxVector2&);
			vec2f(const GLfloat x, const GLfloat y);

			bool operator == (const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;
			bool operator != (const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;

			static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
		};

		struct vec3f
		{
			GLfloat x;
			GLfloat y;
			GLfloat z;

			vec3f() = default;
			vec3f(const GINTONIC_NAMESPACE_FBX::FbxVector4&);
			vec3f(const GLfloat x, const GLfloat y, const GLfloat z);

			bool operator == (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;
			bool operator != (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

			static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
		};

		struct vec4f
		{
			GLfloat x;
			GLfloat y;
			GLfloat z;
			GLfloat w;

			vec4f() = default;
			vec4f(const GINTONIC_NAMESPACE_FBX::FbxVector4&);
			vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

			bool operator == (const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;
			bool operator != (const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;

			static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
		};

		void set_data(GINTONIC_NAMESPACE_FBX::FbxMesh*, const GLenum usagehint = GL_STATIC_DRAW);

		template <class VertexType> 
		mesh(const std::vector<VertexType>& vertices,
			const GLenum usageHint = GL_STATIC_DRAW)
		{
			set_data(vertices, usageHint);
		}

		unsigned type() const BOOST_NOEXCEPT_OR_NOTHROW;

	private:

		opengl::vertex_array_object m_vao;

		// indices, positions, uvs, normals
		opengl::buffer_object_array<4> m_buffer;
		
		// matrix_PVM, matrix_VM, matrix_N
		opengl::vector_array<GL_ARRAY_BUFFER, mat4f, 2> m_matrix_buffer;
		opengl::vector<GL_ARRAY_BUFFER, mat3f> m_matrix_N_buffer;
		
		GLsizei m_count = 0;
		unsigned m_type = 0;
	};

} // namespace gintonic

#endif