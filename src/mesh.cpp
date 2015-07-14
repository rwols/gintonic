#include "mesh.hpp"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

namespace gintonic
{
	mesh::mesh(mesh&& other)
	: object<mesh, key_type>(std::move(other))
	, m_vao(std::move(other.m_vao))
	, m_vbo(std::move(other.m_vbo))
	, m_ibo(std::move(other.m_ibo))
	, m_count(std::move(other.m_count))
	{
		// do nothing
	}

	mesh& mesh::operator = (mesh&& other)
	{
		object<mesh, key_type>::operator=(std::move(other));
		m_vao = std::move(other.m_vao);
		m_vbo = std::move(other.m_vbo);
		m_count = std::move(other.m_count);
		return *this;
	}

	// mesh::key_type mesh::process(boost::filesystem::path p)
	// {
	// 	const auto ext = p.extension();

	// 	if (ext == ".obj" || ext == ".OBJ") process_obj(p);
	// 	else if (ext == ".fbx" || ext == ".FBX") process_fbx(p);
	// 	else
	// 	{
	// 		BOOST_THROW_EXCEPTION(error() << errinfo_path(p));
	// 	}
	// 	return p;
	// }

	// bool mesh::validate(std::ostream& log, const boost::filesystem::path& filename)
	// {
	// 	const auto ext = filename.extension();
	// 	if (ext == ".obj" || ext == ".OBJ") return validate_obj(log, filename);
	// 	else if (ext == ".fbx" || ext == ".FBX") return validate_fbx(log, filename);
	// 	else
	// 	{
	// 		BOOST_THROW_EXCEPTION(error() << errinfo_path(filename));
	// 	}
	// }

	mesh::key_type mesh::process(FbxMesh* m, FbxManager* manager)
	{

		return "";
	}

	// bool mesh::validate_obj(std::ostream& log, boost::filesystem::path filename)
	// {
	// 	std::vector<GLuint> indices;
	// 	std::vector<vec3f> positions, normals;
	// 	std::vector<vec2f> uvs;
	// 	// std::map<GLuint,GLuint> positions_to_normals;
	// 	std::ifstream input(filename.c_str());
	// 	std::string line, mesh_name(filename.stem().string());
	// 	int obj_type = -1;
	// 	while (std::getline(input, line))
	// 	{
	// 		if (line.size() <= 2) continue;
	// 		if (line.substr(0, 2) == "v ")
	// 		{
	// 			std::istringstream ss(line.substr(2));
	// 			vec3f v;
	// 			ss >> v[0];
	// 			ss >> v[1];
	// 			ss >> v[2];
	// 			positions.push_back(v);
	// 		}
	// 		else if (line.substr(0, 3) == "vn ")
	// 		{
	// 			std::istringstream ss(line.substr(3));
	// 			vec3f v;
	// 			ss >> v[0];
	// 			ss >> v[1];
	// 			ss >> v[2];
	// 			normals.push_back(v);
	// 		}
	// 		else if (line.substr(0, 3) == "vt ")
	// 		{
	// 			std::istringstream ss(line.substr(3));
	// 			vec2f uv;
	// 			ss >> uvs[0];
	// 			ss >> uvs[1];
	// 			uvs.push_back(uv);
	// 		}
	// 		else if (line.substr(0, 2) == "f ")
	// 		{
	// 			const auto toks = retrieve_indices_from_line<GLuint>(positions.size(), line.substr(2));

	// 			if (toks.size() == 3)
	// 			{
	// 				obj_type = 0;
	// 				indices.insert(indices.end(), toks.begin(), toks.end());
	// 			}
	// 			else if (toks.size() == 6)
	// 			{
	// 				obj_type = 1;
	// 				indices.push_back(toks[0]);
	// 				indices.push_back(toks[2]);
	// 				indices.push_back(toks[4]);
	// 			}
	// 			else if (toks.size() == 9)
	// 			{
	// 				obj_type = 2;
	// 				indices.push_back(toks[0]);
	// 				indices.push_back(toks[3]);
	// 				indices.push_back(toks[6]);
	// 			}
	// 			else
	// 			{
	// 				log << "The mesh is not fully triangulated. Make sure that during export, the mesh is triangulated.\n";
	// 				log << "If this is not fixed, the mesh will not display correctly.\n";
	// 				return false;
	// 			}
	// 		}
	// 		else if (line.substr(0, 2) == "o ")
	// 		{
	// 			mesh_name = line.substr(2);
	// 		}
	// 	}

	// 	log << "The mesh's name is \"" << mesh_name << "\".\n";
	// 	const char* vertex_type_extension = nullptr;
	
	// 	if (obj_type == 0)
	// 	{
	// 		log << "The mesh contains only position information.\n";
	// 		vertex_type_extension = opengl::vertex_P<GLfloat>::extension();
	// 	}
	// 	else if (obj_type == 1)
	// 	{
	// 		log << "The mesh contains positions and normals.\n";
	// 		vertex_type_extension = opengl::vertex_PN<GLfloat>::extension();
	// 	}
	// 	else if (obj_type == 2)
	// 	{
	// 		log << "The mesh contains positions, UVs and normals.\n";
	// 		vertex_type_extension = opengl::vertex_PUN<GLfloat>::extension();
	// 	}

	// 	log << "The mesh contains\n"
	// 		<< '\t' << indices.size() << " indices,\n"
	// 		<< '\t' << positions.size() << " positions,\n"
	// 		<< '\t' << normals.size() << " normals, and\n"
	// 		<< '\t' << uvs.size() << " UVs.\n";
	
	// 	filename = "..";
	// 	filename /= "resources";
	// 	mesh_name.append(vertex_type_extension);
	// 	filename /= mesh_name;

	// 	log << "The mesh would be saved to: " << filename << '\n';
	// 	log << "Everything looks OK.\n";
	// 	return true;
	// }

	bool mesh::validate(std::ostream& log, FbxMesh* m, FbxManager* manager)
	{
		int i, j, index, polygon_size;
		bool success, unmapped;
		FbxVector4 position;
		FbxVector4 fbx_normal;
		FbxVector2 fbx_uv;

		if (m->IsTriangleMesh() == false)
		{
			log << m->GetNode()->GetName() << " is not fully triangulated.\n";
			return false;
		}
		// FbxGeometryConverter geometry_converter(manager);
		// success = geometry_converter.EmulateNormalsByPolygonVertex(m);
		// if (!success)
		// {
		// 	log << m->GetNode()->GetName() << ": unable to convert geometry.\n";
		// 	return false;
		// }

		std::vector<GLuint> indices(m->GetPolygonVertices(), m->GetPolygonVertices() + m->GetPolygonVertexCount());
		std::vector<vec3f> positions/*(m->GetControlPoints(), m->GetControlPoints() + m->GetControlPointsCount())*/;

		// m->GetElementNormal()->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		// m->GetElementNormal()->SetReferenceMode(FbxLayerElement::eDirect);
		// m->GetElementUV()->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		// m->GetElementUV()->SetReferenceMode(FbxLayerElement::eDirect);

		// const auto& fbx_normals = m->GetElementNormal()->GetDirectArray();
		// const auto& fbx_uvs = m->GetElementUV()->GetDirectArray();
		// std::vector<vec3f> normals(fbx_normals.GetCount());
		// std::vector<vec2f> uvs(fbx_uvs.GetCount());
		// for (std::size_t i = 0; i < fbx_normals.GetCount(); ++i)
		// {
		// 	normals[i] = fbx_normals[i];
		// }
		// for (std::size_t i = 0; i < fbx_uvs.GetCount(); ++i)
		// {
		// 	uvs[i]= fbx_uvs[i];
		// }

		std::vector<vec3f> normals;
		std::vector<vec2f> uvs;

		const int polygon_count = m->GetPolygonCount();
		// const int positions_count = m->GetControlPointsCount();

		FbxStringList uv_names;
		m->GetUVSetNames(uv_names);
		log << "UV Sets:\n";
		for (i = 0; i < uv_names.GetCount(); ++i)
		{
			log << '\t' << uv_names[i] << '\n';
		}

		for (i = 0; i < polygon_count; ++i)
		{
			polygon_size = m->GetPolygonSize(i);
			for (j = 0; j < polygon_size; ++j)
			{
				index = m->GetPolygonVertex(i, j);
				if (index < 0)
				{
					log << "Invalid polygon vertex index: " << i << ", " << j << '\n';
				}
				else
				{
					positions.emplace_back(m->GetControlPointAt(m->GetPolygonVertex(i,j)));
					success = m->GetPolygonVertexNormal(i, j, fbx_normal);
					if (!success)
					{
						log << "Failed to load normal vector: " << i << ", " << j << '\n';
					}
					else
					{
						normals.emplace_back(fbx_normal);
					}
					if (uv_names.GetCount() > 0)
					{
						success = m->GetPolygonVertexUV(i, j, uv_names[0], fbx_uv, unmapped);
						if (unmapped)
						{
							log << "UV at " << i << ", " << j << " is unmapped.\n";	
						}
						else
						{
							if (!success)
							{
								log << "Failed to load UV: " << i << ", " << j << '\n';
							}
							else
							{
								uvs.emplace_back(fbx_uv[0], fbx_uv[1]);
							}
						}
					}
				}
			}
		}

		log << "Indices:\n";
		for (const auto& i : indices)
		{
			log << '\t' << i << '\n';
		}

		log << "\nPositions:\n";
		for (const auto position : positions)
		{
			log << '\t' << position << '\n';
		}
		log << "\nUVs:\n";
		for (const auto x : uvs)
		{
			log << '\t' << x << '\n';
		}
		log << "\nNormals:\n";
		for (const auto n : normals)
		{
			log << '\t' << n << '\n';
		}

		if (uvs.empty())
		{
			log << "No UVs are present.\n";
		}
		else
		{
			typedef opengl::vertex_PUN<GLfloat> vertex_type;
			std::vector<vertex_type> vertices;
			for (std::size_t i = 0; i < positions.size(); ++i)
			{
				vertices.emplace_back(positions[i], uvs[i], normals[i]);
			}
			log << "Vertices:\n";
			for (const auto& v : vertices)
			{
				log << '\t' << v << '\n';
			}
		}

		return true;
	}

	// bool mesh::validate_fbx(std::ostream& log, const boost::filesystem::path& filename)
	// {
	// 	auto* manager = FbxManager::Create();
	// 	auto* iosettings = FbxIOSettings::Create(manager, IOSROOT);
	// 	manager->SetIOSettings(iosettings);
	// 	auto* importer = FbxImporter::Create(manager, "");
	// 	auto import_status = importer->Initialize(filename.c_str(), -1, manager->GetIOSettings());
	// 	if (!import_status)
	// 	{
	// 		log << "Call to FbxImporter::Initialize() failed.\n";
	// 		log << "Error returned: " << importer->GetStatus().GetErrorString() << '\n';
	// 		return false;
	// 	}
	// 	auto* scene = FbxScene::Create(manager ,"untitled");
	// 	importer->Import(scene);
	// 	importer->Destroy();
	// 	return false;

	// }

	void mesh::draw() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	const char* mesh::type() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_type;
	}

	mesh::mesh(const key_type& k) : object<mesh, key_type>(k)
	{
		construct();
	}
	mesh::mesh(key_type&& k) : object<mesh, key_type>(std::move(k))
	{
		construct();
	}

	// void mesh::process_obj(boost::filesystem::path& p)
	// {
	// 	std::vector<GLuint> indices;
	// 	std::vector<vec3f> positions, normals;
	// 	std::vector<vec2f> uvs;
	// 	std::map<GLuint,GLuint> positions_to_normals;
	// 	std::map<GLuint, GLuint> positions_to_uvs;
	// 	std::ifstream input(p.c_str());
	// 	std::string line, mesh_name(p.stem().string());
	// 	int obj_type = -1;
	// 	while (std::getline(input, line))
	// 	{
	// 		if (line.size() <= 2) continue;
	// 		if (line.substr(0, 2) == "v ")
	// 		{
	// 			std::istringstream ss(line.substr(2));
	// 			vec3f v;
	// 			ss >> v[0];
	// 			ss >> v[1];
	// 			ss >> v[2];
	// 			positions.push_back(v);
	// 		}
	// 		else if (line.substr(0, 3) == "vn ")
	// 		{
	// 			std::istringstream ss(line.substr(3));
	// 			vec3f v;
	// 			ss >> v[0];
	// 			ss >> v[1];
	// 			ss >> v[2];
	// 			normals.push_back(v);
	// 		}
	// 		else if (line.substr(0, 3) == "vt ")
	// 		{
	// 			std::istringstream ss(line.substr(3));
	// 			vec2f uv;
	// 			ss >> uv[0];
	// 			ss >> uv[1];
	// 			uvs.push_back(uv);
	// 		}
	// 		else if (line.substr(0, 2) == "f ")
	// 		{
	// 			const auto toks = retrieve_indices_from_line<GLuint>(positions.size(), line.substr(2));

	// 			// put map: position index -> normal index here

	// 			if (toks.size() == 3)
	// 			{
	// 				obj_type = 0;
	// 				indices.insert(indices.end(), toks.begin(), toks.end());
	// 			}
	// 			else if (toks.size() == 6)
	// 			{
	// 				obj_type = 1;
	// 				positions_to_normals[toks[0]] = toks[1];
	// 				positions_to_normals[toks[2]] = toks[3];
	// 				positions_to_normals[toks[4]] = toks[5];
	// 				indices.push_back(toks[0]);
	// 				indices.push_back(toks[2]);
	// 				indices.push_back(toks[4]);
	// 			}
	// 			else if (toks.size() == 9)
	// 			{
	// 				obj_type = 2;
	// 				positions_to_normals[toks[0]] = toks[2];
	// 				positions_to_normals[toks[3]] = toks[5];
	// 				positions_to_normals[toks[6]] = toks[8];
	// 				positions_to_uvs[toks[0]] = toks[1];
	// 				positions_to_uvs[toks[3]] = toks[4];
	// 				positions_to_uvs[toks[6]] = toks[7];
	// 				indices.push_back(toks[0]);
	// 				indices.push_back(toks[3]);
	// 				indices.push_back(toks[6]);
	// 			}
	// 		}
	// 		else if (line.substr(0, 2) == "o ")
	// 		{
	// 			mesh_name = line.substr(2);
	// 		}
	// 	}

	// 	const auto filename_copy = p;
	// 	p = "..";
	// 	p /= "resources";

	// 	if (obj_type == 0)
	// 	{
	// 		typedef opengl::vertex_P<GLfloat> vertex_type;
	// 		mesh_name.append(vertex_type::extension());
	// 		p /= mesh_name;
	// 		std::vector<vertex_type> vertices(positions.size());
	// 		for (std::size_t index = 0; index < positions.size(); ++index)
	// 		{
	// 			vertices[index] = vertex_type(positions[index]);
	// 		}
	// 		std::ofstream output(p.c_str(), std::ios::binary);
	// 		eos::portable_oarchive oa(output);
	// 		oa & vertices;
	// 		oa & indices;	
	// 	}
	// 	else if (obj_type == 1)
	// 	{
	// 		typedef opengl::vertex_PN<GLfloat> vertex_type;
	// 		mesh_name.append(vertex_type::extension());
	// 		p /= mesh_name;
	// 		std::vector<vertex_type> vertices(positions.size());
	// 		for (std::size_t index = 0; index < positions.size(); ++index)
	// 		{
	// 			vertices[index] = vertex_type(positions[index], normals[positions_to_normals[index]]);
	// 		}
	// 		std::ofstream output(p.c_str(), std::ios::binary);
	// 		if (!output)
	// 		{
	// 			throw std::runtime_error("Could not open file for binary write.");
	// 		}
	// 		eos::portable_oarchive oa(output);
	// 		oa & vertices;
	// 		oa & indices;
	// 	}
	// 	else if (obj_type == 2)
	// 	{
	// 		typedef opengl::vertex_PUN<GLfloat> vertex_type;
	// 		mesh_name.append(vertex_type::extension());
	// 		p /= mesh_name;
	// 		std::vector<vertex_type> vertices(positions.size());
	// 		for (std::size_t index = 0; index < positions.size(); ++index)
	// 		{
	// 			vertices[index] = vertex_type(positions[index], uvs[positions_to_uvs[index]], normals[positions_to_normals[index]]);
	// 		}
	// 		std::ofstream output(p.c_str(), std::ios::binary);
	// 		if (!output)
	// 		{
	// 			throw std::runtime_error("Could not open file for binary write.");
	// 		}
	// 		eos::portable_oarchive oa(output);
	// 		oa & vertices;
	// 		oa & indices;
	// 	}
	// 	else
	// 	{
	// 		BOOST_THROW_EXCEPTION(error() << errinfo_path(filename_copy));
	// 	}
	// }

	// void mesh::process_fbx(boost::filesystem::path& p)
	// {
	// 	throw std::runtime_error("mesh::process_fbx: not yet implemented.");
	// }

	void mesh::construct()
	{
		DEBUG_GL_ERROR;
		std::ifstream input(key().c_str(), std::ios::binary);
		DEBUG_GL_ERROR;
		eos::portable_iarchive ia(input);
		DEBUG_GL_ERROR;
		const auto ext = key().extension();
		DEBUG_GL_ERROR;
		std::vector<GLuint> indices;
		DEBUG_GL_ERROR;
		if (ext == opengl::vertex_P<GLfloat>::extension())
		{
			std::vector<opengl::vertex_P<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PC<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PC<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PU<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PU<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PN<GLfloat>::extension())
		{
			DEBUG_GL_ERROR;
			std::vector<opengl::vertex_PN<GLfloat>> vertices;
			DEBUG_GL_ERROR;
			ia & vertices;
			DEBUG_GL_ERROR;
			ia & indices;
			DEBUG_GL_ERROR;
			set_data(vertices, indices);
			DEBUG_GL_ERROR;
		}
		else if (ext == opengl::vertex_PUN<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PUN<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PCU<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PCU<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PUNTB<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PUNTB<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PCUNTB<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PCUNTB<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else if (ext == opengl::vertex_PCUN<GLfloat>::extension())
		{
			std::vector<opengl::vertex_PCUN<GLfloat>> vertices;
			ia & vertices;
			ia & indices;
			set_data(vertices, indices);
		}
		else
		{
			BOOST_THROW_EXCEPTION(unknown_vertex_format() << errinfo_path(key()));
		}
	}
} // namespace gintonic