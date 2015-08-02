#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "vertices.hpp"

namespace gintonic {
	
	class mesh : public std::enable_shared_from_this<mesh>
	{
	public:

		struct error : virtual exception {};
		struct unknown_vertex_format : virtual error {};

		mesh();
		mesh(FbxMesh*, const GLenum usageHint = GL_STATIC_DRAW);
		
		mesh(const mesh&) = delete;
		mesh(mesh&& other);
		mesh& operator = (const mesh&) = delete;
		mesh& operator = (mesh&& other);

		void draw() const BOOST_NOEXCEPT_OR_NOTHROW;

		template <class VertexType>
		void retrieve_data(std::vector<VertexType>& vertices 
			/*std::vector<GLuint>& indices*/) const
		{
			static_assert(
				opengl::has_enable_attributes_method<VertexType>::value, 
				"Must be a vertex type.");
			// GLint array_buffer_size;

			glBindVertexArray(m_vao);

			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

			// glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, 
			// 	&array_buffer_size);

			vertices.resize(m_count);

			// vertices.resize(array_buffer_size / sizeof(VertexType));
			// indices.resize(m_count);

			glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(VertexType), 
				vertices.data());

			// glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 
			// 	m_count * sizeof(GLuint), indices.data());

			// glBindBuffer(GL_ARRAY_BUFFER, 0);

			// glBindVertexArray(0);
		}

		template <class VertexType>
		void set_data(const std::vector<VertexType>& vertices, 
			// const std::vector<GLuint>& indices, 
			const GLenum usageHint = GL_STATIC_DRAW)
		{
			m_vao.setup(m_vbo, vertices, /*indices,*/ usageHint);
			// m_count = static_cast<GLsizei>(indices.size());
			m_count = static_cast<GLsizei>(vertices.size());
			m_type = VertexType::extension();
		}

		void set_data(FbxMesh*, const GLenum usageHint = GL_STATIC_DRAW);

		template <class VertexType> 
		mesh(const std::vector<VertexType>& vertices,
			const GLenum usageHint = GL_STATIC_DRAW)
		{
			set_data(vertices, usageHint);
		}

		const char* type() const BOOST_NOEXCEPT_OR_NOTHROW;

	private:

		// friend boost::flyweights::detail::optimized_key_value<key_type, mesh, key_extractor>;
		// virtual void construct_from_key() final;

		// friend boost::serialization::access;

		// template <class Archive> void serialize(Archive& ar, 
		// 	const unsigned int /*version*/)
		// {
		// 	using boost::serialization::base_object;
		// 	ar & base_object<object<mesh,key_type>>(*this);
		// 	ar & m_vao & m_vbo & /* ibo & */ m_count & m_type;
		// }

		// template <class Archive> void save(Archive& ar, 
		// 	const unsigned int /*version*/) const
		// {
		// 	ar & key();
		// }

		// template <class Archive> void load(Archive& ar,
		// 	const unsigned int /*version*/)
		// {
		// 	key_type key;
		// 	ar & key;
		// }
		// BOOST_SERIALIZATION_SPLIT_MEMBER();

		opengl::vertex_array_object m_vao;
		opengl::buffer_object m_vbo;
		// opengl::buffer_object m_ibo;
		GLsizei m_count = 0;
		const char* m_type = nullptr;
	};

} // namespace gintonic

#endif