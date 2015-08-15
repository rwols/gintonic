#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "vertices.hpp"

namespace gintonic {

	class matrix_stack
	{
	public:
		template <class ...Args> void set_model_matrix(Args&& ...args)
		{
			m_matrix_VM = renderer::camera().matrix_V() * mat4f(std::forward<Args>(args)...);
			m_matrix_PVM = renderer::camera().matrix_P(renderer::width(), renderer::height()) * m_matrix_VM;
			m_matrix_N = m_matrix_VM.upper_left_33().invert().transpose();
		}

		inline const mat4f& matrix_PVM() const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return m_matrix_PVM;
		}

		inline const mat4f& matrix_VM() const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return m_matrix_VM;
		}

		inline const mat3f& matrix_N() const BOOST_NOEXCEPT_OR_NOTHROW
		{
			return m_matrix_N;
		}

	private:
		mat4f m_matrix_PVM;
		mat4f m_matrix_VM;
		mat3f m_matrix_N;
	};
	
	class mesh : public std::enable_shared_from_this<mesh>
	{
	public:

		mesh() = default;
		mesh(FbxMesh*, const GLenum usagehint = GL_STATIC_DRAW);
		virtual ~mesh();

		void draw() const BOOST_NOEXCEPT_OR_NOTHROW;
		void draw(const std::vector<std::tuple<mat4f, mat4f, mat3f>>& matrices) BOOST_NOEXCEPT_OR_NOTHROW;

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

		// template <class VertexType>
		// void retrieve_data(std::vector<VertexType>& vertices 
		// 	/*std::vector<GLuint>& indices*/) const
		// {
		// 	static_assert(
		// 		opengl::has_enable_attributes_method<VertexType>::value, 
		// 		"Must be a vertex type.");
		// 	// GLint array_buffer_size;

		// 	glBindVertexArray(m_vao);

		// 	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		// 	// glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, 
		// 	// 	&array_buffer_size);

		// 	vertices.resize(m_count);

		// 	// vertices.resize(array_buffer_size / sizeof(VertexType));
		// 	// indices.resize(m_count);

		// 	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(VertexType), 
		// 		vertices.data());

		// 	// glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 
		// 	// 	m_count * sizeof(GLuint), indices.data());

		// 	// glBindBuffer(GL_ARRAY_BUFFER, 0);

		// 	// glBindVertexArray(0);
		// }

		// template <class VertexType>
		// void set_data(const std::vector<VertexType>& vertices, 
		// 	// const std::vector<GLuint>& indices, 
		// 	const GLenum usageHint = GL_STATIC_DRAW)
		// {
		// 	m_vao.setup(m_vbo, vertices, /*indices,*/ usageHint);
		// 	// m_count = static_cast<GLsizei>(indices.size());
		// 	m_count = static_cast<GLsizei>(vertices.size());
		// 	m_type = VertexType::extension();
		// }

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
		// opengl::buffer_object m_vbo;

		// indices, positions, uvs, normals, matrix_PVM, matrix_VM, matrix_N
		opengl::buffer_object_array<7> m_buffer;
		// opengl::buffer_object m_ibo;
		GLsizei m_count = 0;
		unsigned m_type = 0;
	};

} // namespace gintonic

#endif