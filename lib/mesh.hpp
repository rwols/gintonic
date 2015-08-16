#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "vertices.hpp"

namespace gintonic {
	
	class mesh : public std::enable_shared_from_this<mesh>
	{
	public:

		mesh() = default;
		mesh(FbxMesh*, const GLenum usagehint = GL_STATIC_DRAW);

		void draw() const BOOST_NOEXCEPT_OR_NOTHROW;

		void draw(
			const std::vector<mat4f>& PVM_matrices,
			const std::vector<mat4f>& VM_matrices,
			const std::vector<mat3f>& N_matrices)
			const BOOST_NOEXCEPT_OR_NOTHROW;

		struct vec2f
		{
			GLfloat x;
			GLfloat y;

			vec2f() = default;
			vec2f(const FbxVector2&);
			vec2f(const GLfloat x, const GLfloat y);

			static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
		};

		struct vec3f
		{
			GLfloat x;
			GLfloat y;
			GLfloat z;

			vec3f() = default;
			vec3f(const FbxVector4&);
			vec3f(const GLfloat x, const GLfloat y, const GLfloat z);

			static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
		};

		struct vec4f
		{
			GLfloat x;
			GLfloat y;
			GLfloat z;
			GLfloat w;

			vec4f() = default;
			vec4f(const FbxVector4&);
			vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

			static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
		};

		void set_data(FbxMesh*, const GLenum usagehint = GL_STATIC_DRAW);

		template <class VertexType> 
		mesh(const std::vector<VertexType>& vertices,
			const GLenum usageHint = GL_STATIC_DRAW)
		{
			set_data(vertices, usageHint);
		}

		unsigned type() const BOOST_NOEXCEPT_OR_NOTHROW;

	private:

		opengl::vertex_array_object m_vao;

		// indices, positions, uvs, normals, matrix_PVM, matrix_VM, matrix_N
		opengl::buffer_object_array<7> m_buffer;
		GLsizei m_count = 0;
		unsigned m_type = 0;
	};

} // namespace gintonic

#endif