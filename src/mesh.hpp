#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "opengl.hpp"
#include "object.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

namespace gintonic
{
	template <class VertexType>
	class mesh : public object<mesh<VertexType>, boost::filesystem::path>
	{

	public:

		typedef VertexType vertex_type;
		typedef typename vertex_type::float_type float_type;
		typedef typename object<mesh, boost::filesystem::path>::key_type key_type;
		typedef typename object<mesh, key_type>::key_extractor key_extractor;
		using object<mesh, key_type>::key;

		mesh() = delete;
		mesh(const mesh&) = delete;
		mesh(mesh&& other)
		: object<mesh, key_type>(std::move(other))
		, m_vao(std::move(other.m_vao))
		, m_vbo(std::move(other.m_vbo))
		, m_ibo(std::move(other.m_ibo))
		, m_count(std::move(other.m_count))
		{
			// do nothing
		}

		mesh& operator = (const mesh&) = delete;
		mesh& operator = (mesh&& other)
		{
			object<mesh, key_type>::operator=(std::move(other));
			m_vao = std::move(other.m_vao);
			m_vbo = std::move(other.m_vbo);
			m_count = std::move(other.m_count);
			return *this;
		}

		static boost::filesystem::path process(boost::filesystem::path p)
		{
		
			const auto ext = p.extension();
			std::vector<vertex_type> vertices;
			std::vector<GLushort> indices;

			if (ext == ".obj" || ext == ".OBJ") process_obj(p, vertices, indices);
			else if (ext == ".fbx" || ext == ".FBX") process_fbx(p, vertices, indices);
			else throw std::runtime_error("mesh::process: Unknown file extension.");
		
			std::ofstream output(p.c_str(), std::ios::binary);
		
			if (!output)
			{
				throw std::runtime_error("mesh::process: Cannot write to file.");
			}
			{
				eos::portable_oarchive oa(output);
				oa & vertices;
				oa & indices;
			}
			return p;
		}

		void draw() const BOOST_NOEXCEPT_OR_NOTHROW
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_SHORT, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void retrieve_data(std::vector<vertex_type>& vertices, std::vector<GLushort>& indices) const
		{
			GLint array_buffer_size;
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &array_buffer_size);
			vertices.resize(array_buffer_size / sizeof(vertex_type));
			indices.resize(m_count);
			glGetBufferSubData(GL_ARRAY_BUFFER, 0, array_buffer_size, vertices.data());
			glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_count * sizeof(GLushort), indices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

	private:

		mesh(const key_type& name, const std::vector<vertex_type>& vertices, const std::vector<GLushort> indices)
		: object<mesh, key_type>(name)
		{
			m_count = static_cast<GLsizei>(indices.size());
			m_num_vertices = static_cast<GLsizei>(vertices.size());
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			vertex_type::EnableVertexAttribArray();
			gtBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_READ);
			gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_READ);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void set_data(const std::vector<vertex_type>& vertices, const std::vector<GLushort>& indices)
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			gtBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
			gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			m_count = static_cast<GLsizei>(indices.size());
		}

		mesh(const key_type& k) : object<mesh, key_type>(k)
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			vertex_type::EnableVertexAttribArray();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			init_class();
		}
		mesh(key_type&& k) : object<mesh, key_type>(std::move(k))
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			vertex_type::EnableVertexAttribArray();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			init_class();
		}

		friend boost::flyweights::detail::optimized_key_value<key_type, mesh, key_extractor>;

		static void process_obj(boost::filesystem::path& p, 
			std::vector<vertex_type>& vertices, std::vector<GLushort>& indices)
		{
			std::vector<vec3f> positions, normals;
			std::map<GLushort,GLushort> positions_to_normals;
			std::ifstream input(p.c_str());
			std::string line, mesh_name("untitled");
			int obj_type = -1;
			while (std::getline(input, line))
			{
				if (line.size() <= 2) continue;
				if (line.substr(0, 2) == "v ")
				{
					std::istringstream ss(line.substr(2));
					vec3f v;
					ss >> v[0];
					ss >> v[1];
					ss >> v[2];
					positions.push_back(v);
				}
				else if (line.substr(0, 3) == "vn ")
				{
					std::istringstream ss(line.substr(3));
					vec3f v;
					ss >> v[0];
					ss >> v[1];
					ss >> v[2];
					normals.push_back(v);
				}
				else if (line.substr(0, 2) == "f ")
				{
					const auto toks = retrieve_indices_from_line<GLushort>(line.substr(2));

					// put map: position index -> normal index here

					if (toks.size() == 3)
					{
						obj_type = 0;
						indices.insert(indices.end(), toks.begin(), toks.end());
					}
					else if (toks.size() == 6)
					{
						obj_type = 1;
						positions_to_normals[toks[0]] = toks[1];
						positions_to_normals[toks[2]] = toks[3];
						positions_to_normals[toks[4]] = toks[5];
						indices.push_back(toks[0]);
						indices.push_back(toks[2]);
						indices.push_back(toks[4]);
					}
					else if (toks.size() == 9)
					{
						obj_type = 2;
					}
				}
				else if (line.substr(0, 2) == "o ")
				{
					mesh_name = line.substr(2);
				}
			}

			vertices.resize(positions.size());
		
			const vec4f dummycolor(1,0,0,1);
			const vec3f dummynormal(0,1,0);
			const vec3f dummytangent(1,0,0);
			const vec3f dummybinormal(0,0,1);
			const vec2f dummyUV(0,0);

			if (obj_type == 0)
			{
				std::cout << "positions\n";
				for (std::size_t index = 0; index < positions.size(); ++index)
				{
					vertices[index] = vertex_type(positions[index], dummycolor, dummyUV, dummynormal, dummytangent, dummybinormal);
				}	
			}
			else if (obj_type == 1)
			{
				std::cout << "positions and normals\n";
				for (std::size_t index = 0; index < positions.size(); ++index)
				{
					vertices[index] = vertex_type(positions[index], dummycolor, dummyUV, normals[positions_to_normals[index]], dummytangent, dummybinormal);
				}	
			}
			else if (obj_type == 2)
			{
				std::cout << "positions, UVs and normals\n";
				throw std::runtime_error("not yet implemented (obj_type == 2)");
			}
		
			p = "..";
			p /= "resources";
			mesh_name.append(vertex_type::extension());
			p /= mesh_name;

			std::ofstream output(p.c_str(), std::ios::binary);
			{
				eos::portable_oarchive oa(output);
				oa & vertices;
				oa & indices;
			}
		}

		static void process_fbx(const boost::filesystem::path& p, 
			std::vector<vertex_type>& vertices, std::vector<GLushort>& indices)
		{
			throw std::runtime_error("mesh::process_fbx: not yet implemented.");
		}

		void init_class()
		{
			if (key().extension() != vertex_type::extension())
			{
			
				throw std::runtime_error("mesh::init_class: vertex type mismatch.");
			}
		
			std::ifstream input(key().c_str(), std::ios::binary);
			if (!input)
			{
				throw std::runtime_error("mesh::init_class: could not open file.");
			}
		
			std::vector<vertex_type> vertices;
			std::vector<GLushort> indices;
			{
			
				eos::portable_iarchive ia(input);
				ia & vertices;
				ia & indices;
			}

			set_data(vertices, indices);
		}

		template <class ResultType> static std::vector<ResultType> retrieve_indices_from_line(const std::string& line)
		{
			boost::tokenizer<> tok(line);
			std::vector<ResultType> result;
			std::transform(tok.begin(), tok.end(), std::back_inserter(result), [](const std::string& str)
			{
				return boost::lexical_cast<ResultType>(str) - 1;
			});
			return result;
		}

		opengl::vertex_array_object m_vao;
		opengl::buffer_object m_vbo;
		opengl::buffer_object m_ibo;
		GLsizei m_count = 0;
		GLsizei m_num_vertices = 0;
	};
} // namespace gintonic

#endif