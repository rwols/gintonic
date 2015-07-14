#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include <fstream>
#include "opengl.hpp"
#include "object.hpp"
#include "vertices.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

namespace gintonic
{
	class mesh : public object<mesh, boost::filesystem::path>
	{

	public:

		struct error : virtual exception {};
		struct unknown_vertex_format : virtual error {};

		// typedef VertexType vertex_type;
		// typedef typename vertex_type::float_type float_type;
		// typedef typename object<mesh, boost::filesystem::path>::key_type key_type;
		// typedef typename object<mesh, key_type>::key_extractor key_extractor;
		// using object<mesh, key_type>::key;

		mesh() = delete;
		mesh(const mesh&) = delete;
		mesh(mesh&& other);

		mesh& operator = (const mesh&) = delete;
		mesh& operator = (mesh&& other);

		static boost::filesystem::path process(boost::filesystem::path p)
		{
			const auto ext = p.extension();

			if (ext == ".obj" || ext == ".OBJ") process_obj(p);
			else if (ext == ".fbx" || ext == ".FBX") process_fbx(p);
			else throw std::runtime_error("mesh::process: Unknown file extension.");
		
			return p;
		}

		static key_type process(FbxMesh*, FbxManager*);
		static bool validate(std::ostream&, FbxMesh*, FbxManager*);

		static bool validate(std::ostream& log, const boost::filesystem::path& filename)
		{
			const auto ext = filename.extension();
			if (ext == ".obj" || ext == ".OBJ") return validate_obj(log, filename);
			else if (ext == ".fbx" || ext == ".FBX") return validate_fbx(log, filename);
			else throw std::runtime_error("mesh::validate: Unknown file extension.");
		}

		static bool validate_obj(std::ostream& log, boost::filesystem::path filename)
		{
			std::vector<GLuint> indices;
			std::vector<vec3f> positions, normals;
			std::vector<vec2f> uvs;
			// std::map<GLuint,GLuint> positions_to_normals;
			std::ifstream input(filename.c_str());
			std::string line, mesh_name(filename.stem().string());
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
				else if (line.substr(0, 3) == "vt ")
				{
					std::istringstream ss(line.substr(3));
					vec2f uv;
					ss >> uvs[0];
					ss >> uvs[1];
					uvs.push_back(uv);
				}
				else if (line.substr(0, 2) == "f ")
				{
					const auto toks = retrieve_indices_from_line<GLuint>(positions.size(), line.substr(2));

					if (toks.size() == 3)
					{
						obj_type = 0;
						indices.insert(indices.end(), toks.begin(), toks.end());
					}
					else if (toks.size() == 6)
					{
						obj_type = 1;
						indices.push_back(toks[0]);
						indices.push_back(toks[2]);
						indices.push_back(toks[4]);
					}
					else if (toks.size() == 9)
					{
						obj_type = 2;
						indices.push_back(toks[0]);
						indices.push_back(toks[3]);
						indices.push_back(toks[6]);
					}
					else
					{
						log << "The mesh is not fully triangulated. Make sure that during export, the mesh is triangulated.\n";
						log << "If this is not fixed, the mesh will not display correctly.\n";
						return false;
					}
				}
				else if (line.substr(0, 2) == "o ")
				{
					mesh_name = line.substr(2);
				}
			}

			log << "The mesh's name is \"" << mesh_name << "\".\n";
			const char* vertex_type_extension = nullptr;
		
			if (obj_type == 0)
			{
				log << "The mesh contains only position information.\n";
				vertex_type_extension = opengl::vertex_P<GLfloat>::extension();
			}
			else if (obj_type == 1)
			{
				log << "The mesh contains positions and normals.\n";
				vertex_type_extension = opengl::vertex_PN<GLfloat>::extension();
			}
			else if (obj_type == 2)
			{
				log << "The mesh contains positions, UVs and normals.\n";
				vertex_type_extension = opengl::vertex_PUN<GLfloat>::extension();
			}

			log << "The mesh contains\n"
				<< '\t' << indices.size() << " indices,\n"
				<< '\t' << positions.size() << " positions,\n"
				<< '\t' << normals.size() << " normals, and\n"
				<< '\t' << uvs.size() << " UVs.\n";
		
			filename = "..";
			filename /= "resources";
			mesh_name.append(vertex_type_extension);
			filename /= mesh_name;

			log << "The mesh would be saved to: " << filename << '\n';
			log << "Everything looks OK.\n";
			return true;
		}

		static bool validate_fbx(std::ostream& log, const boost::filesystem::path& filename)
		{
			return false;
		}

		void draw() const BOOST_NOEXCEPT_OR_NOTHROW;

		template <class UniversalVertexType>
		void retrieve_data(std::vector<UniversalVertexType>& vertices, std::vector<GLuint>& indices) const
		{
			static_assert(opengl::has_EnableVertexAttribArray_method<UniversalVertexType>::value, "Must be a vertex type.");
			GLint array_buffer_size;
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &array_buffer_size);
			vertices.resize(array_buffer_size / sizeof(UniversalVertexType));
			indices.resize(m_count);
			glGetBufferSubData(GL_ARRAY_BUFFER, 0, array_buffer_size, vertices.data());
			glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_count * sizeof(GLuint), indices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		template <class UniversalVertexType>
		void set_data(const std::vector<UniversalVertexType>& vertices, const std::vector<GLuint>& indices, const GLenum usageHint = GL_STATIC_DRAW)
		{
			static_assert(opengl::has_EnableVertexAttribArray_method<UniversalVertexType>::value, "Must be a vertex type.");
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			UniversalVertexType::EnableVertexAttribArray();
			gtBufferData(GL_ARRAY_BUFFER, vertices, usageHint);
			gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, usageHint);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			m_count = static_cast<GLsizei>(indices.size());
			m_type = UniversalVertexType::extension();
		}

		const char* type() const BOOST_NOEXCEPT_OR_NOTHROW;

		// template <class VertexType>
		// static void register_vertex()
		// {
		// 	static std::set<const char*> s_vertex_types;
		// 	s_vertex_types.insert(VertexType::extension());
		// }

	private:

		mesh(const key_type& k);
		mesh(key_type&& k);

		friend boost::flyweights::detail::optimized_key_value<key_type, mesh, key_extractor>;

		static void process_obj(boost::filesystem::path& p)
		{
			std::vector<GLuint> indices;
			std::vector<vec3f> positions, normals;
			std::vector<vec2f> uvs;
			std::map<GLuint,GLuint> positions_to_normals;
			std::map<GLuint, GLuint> positions_to_uvs;
			std::ifstream input(p.c_str());
			std::string line, mesh_name(p.stem().string());
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
				else if (line.substr(0, 3) == "vt ")
				{
					std::istringstream ss(line.substr(3));
					vec2f uv;
					ss >> uv[0];
					ss >> uv[1];
					uvs.push_back(uv);
				}
				else if (line.substr(0, 2) == "f ")
				{
					const auto toks = retrieve_indices_from_line<GLuint>(positions.size(), line.substr(2));

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
						positions_to_normals[toks[0]] = toks[2];
						positions_to_normals[toks[3]] = toks[5];
						positions_to_normals[toks[6]] = toks[8];
						positions_to_uvs[toks[0]] = toks[1];
						positions_to_uvs[toks[3]] = toks[4];
						positions_to_uvs[toks[6]] = toks[7];
						indices.push_back(toks[0]);
						indices.push_back(toks[3]);
						indices.push_back(toks[6]);
					}
				}
				else if (line.substr(0, 2) == "o ")
				{
					mesh_name = line.substr(2);
				}
			}

			p = "..";
			p /= "resources";
			



			if (obj_type == 0)
			{
				typedef opengl::vertex_P<GLfloat> vertex_type;
				mesh_name.append(vertex_type::extension());
				p /= mesh_name;
				std::vector<vertex_type> vertices(positions.size());
				for (std::size_t index = 0; index < positions.size(); ++index)
				{
					vertices[index] = vertex_type(positions[index]);
				}
				std::ofstream output(p.c_str(), std::ios::binary);
				eos::portable_oarchive oa(output);
				oa & vertices;
				oa & indices;	
			}
			else if (obj_type == 1)
			{
				typedef opengl::vertex_PN<GLfloat> vertex_type;
				mesh_name.append(vertex_type::extension());
				p /= mesh_name;
				std::vector<vertex_type> vertices(positions.size());
				for (std::size_t index = 0; index < positions.size(); ++index)
				{
					vertices[index] = vertex_type(positions[index], normals[positions_to_normals[index]]);
				}
				std::ofstream output(p.c_str(), std::ios::binary);
				if (!output)
				{
					throw std::runtime_error("Could not open file for binary write.");
				}
				eos::portable_oarchive oa(output);
				oa & vertices;
				oa & indices;
			}
			else if (obj_type == 2)
			{
				typedef opengl::vertex_PUN<GLfloat> vertex_type;
				mesh_name.append(vertex_type::extension());
				p /= mesh_name;
				std::vector<vertex_type> vertices(positions.size());
				for (std::size_t index = 0; index < positions.size(); ++index)
				{
					vertices[index] = vertex_type(positions[index], uvs[positions_to_uvs[index]], normals[positions_to_normals[index]]);
				}
				std::ofstream output(p.c_str(), std::ios::binary);
				if (!output)
				{
					throw std::runtime_error("Could not open file for binary write.");
				}
				eos::portable_oarchive oa(output);
				oa & vertices;
				oa & indices;
			}
			else
			{
				throw std::runtime_error("Could not determine obj type of .obj file.");
			}
		}

		static void process_fbx(boost::filesystem::path& p)
		{

			throw std::runtime_error("mesh::process_fbx: not yet implemented.");
		}

		void construct();
		// void construct()
		// {
		// 	std::ifstream input(key().c_str(), std::ios::binary);
		// 	if (!input)
		// 	{
		// 		throw std::runtime_error("mesh::construct: could not open file.");
		// 	}
		// 	const auto ext = key().extension();
		// 	std::vector<GLuint> indices;
		// 	if (ext == opengl::vertex_P<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_P<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PC<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PC<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PU<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PU<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PN<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PN<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PUN<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PUN<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PCU<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PCU<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PUNTB<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PUNTB<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PCUNTB<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PCUNTB<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else if (ext == opengl::vertex_PCUN<GLfloat>::extension())
		// 	{
		// 		std::vector<opengl::vertex_PCUN<GLfloat>> vertices;
		// 		eos::portable_iarchive ia(input);
		// 		ia & vertices;
		// 		ia & indices;
		// 		set_data(vertices, indices);
		// 	}
		// 	else
		// 	{
		// 		throw std::runtime_error("mesh::construct: unknown vertex format");
		// 	}
		// }

		template <class ResultType> static std::vector<ResultType> 
		retrieve_indices_from_line(const std::size_t num_indices, const std::string& line)
		{
			const boost::tokenizer<boost::char_separator<char>> tok(line, boost::char_separator<char>(" /"));
			std::vector<ResultType> result;
			std::transform(tok.begin(), tok.end(), std::back_inserter(result), 
				[&num_indices] (const std::string& str)
			{
				const int index = boost::lexical_cast<int>(str);
				if (index > 0)
				{
					// std::cout << "Found positive index " << index << '\n';
					return static_cast<ResultType>(index - 1);
				}
				else
				{
					const auto actual_index = num_indices + index;
					assert(actual_index >= 0);
					return static_cast<ResultType>(actual_index);
				}
			});
			return result;
		}

		friend boost::serialization::access;
		template <class Archive> void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::base_object<object<mesh, key_type>>(*this);
		}
		template <class Archive> void load(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<object<mesh, key_type>>(*this);
			construct();
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER();

		opengl::vertex_array_object m_vao;
		opengl::buffer_object m_vbo;
		opengl::buffer_object m_ibo;
		GLsizei m_count = 0;
		const char* m_type = nullptr;
	};
} // namespace gintonic

#endif