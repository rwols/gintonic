#include "mesh.hpp"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

namespace
{

void load_UVs(FbxMesh* pMesh, std::vector<gintonic::vec2f>& uvs)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);
	
	// Assume index 0 is the correct one.
	const char* lUVSetName = lUVSetNameList.GetStringAt(0);
	const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

	if(!lUVElement) throw std::runtime_error("Could not fetch geometry element UV.");

	// only support mapping mode eByPolygonVertex and eByControlPoint
	if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
		lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
	{
		throw std::runtime_error("Mapping mode of UVs not supported.\n");		
	}

	//index array, where holds the index referenced to the uv data
	const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
	const int lIndexCount= (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

	//iterating through the data by polygon
	const int lPolyCount = pMesh->GetPolygonCount();

	if( lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
	{
		for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
		{
			// build the max index array that we need to pass into MakePoly
			const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
			for( int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex )
			{
				FbxVector2 lUVValue;

				//get the index of the current vertex in control points array
				int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex,lVertIndex);

				//the UV index depends on the reference mode
				int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

				lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

				//User TODO:
				//Print out the value of UV(lUVValue) or log it to a file
				uvs.emplace_back(static_cast<float>(lUVValue[0]), static_cast<float>(lUVValue[1]));
			}
		}
	}
	else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int lPolyIndexCounter = 0;
		for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
		{
			// build the max index array that we need to pass into MakePoly
			const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
			for( int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex )
			{
				if (lPolyIndexCounter < lIndexCount)
				{
					FbxVector2 lUVValue;

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file
					uvs.emplace_back(static_cast<float>(lUVValue[0]), static_cast<float>(lUVValue[1]));

					lPolyIndexCounter++;
				}
			}
		}
	}
}

} // anonymous namespace

namespace gintonic
{

	mesh::mesh() : m_count(0) {}

	// mesh::mesh(boost::filesystem::path filename)
	// {
	// 	std::ifstream input(filename.c_str(), std::ios::binary);
	// 	eos::portable_iarchive ia(input);
	// 	const auto ext = filename.extension();
	// 	// std::vector<GLuint> indices;
	// 	if (ext == opengl::vertex_P<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_P<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PC<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PC<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PU<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PU<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PN<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PN<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PUN<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PUN<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PCU<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PCU<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PUNTB<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PUNTB<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PCUNTB<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PCUNTB<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PCUN<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PCUN<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else
	// 	{
	// 		throw std::runtime_error("mesh::mesh: unknown vertex format.");
	// 	}
	// }

	void mesh::set_data(FbxMesh* m, const GLenum usageHint)
	{
		int i, j, index, polygon_size;
		bool success;
		FbxVector4 position;
		FbxVector4 fbx_normal;
		FbxVector2 fbx_uv;

		if (m->IsTriangleMesh() == false)
		{
			throw std::runtime_error("mesh::mesh: not fully triangulated.");
		}

		std::vector<GLuint> indices(m->GetPolygonVertices(), 
			m->GetPolygonVertices() + m->GetPolygonVertexCount());
		std::vector<vec3f> positions;
		std::vector<vec3f> normals;
		std::vector<vec2f> uvs;

		const int polygon_count = m->GetPolygonCount();

		FbxStringList uv_names;
		m->GetUVSetNames(uv_names);
		for (i = 0; i < polygon_count; ++i)
		{
			polygon_size = m->GetPolygonSize(i);
			for (j = 0; j < polygon_size; ++j)
			{
				index = m->GetPolygonVertex(i, j);
				if (index < 0)
				{
					throw std::runtime_error("Invalid polyon vertex index.");
				}
				else
				{
					positions.emplace_back(
						m->GetControlPointAt(m->GetPolygonVertex(i,j)));
					success = m->GetPolygonVertexNormal(i, j, fbx_normal);
					if (!success)
					{
						throw std::runtime_error(
							"Failed to load normal vector.");
					}
					else
					{
						normals.emplace_back(fbx_normal);
					}
				}
			}
		}

		if (uv_names.GetCount() > 0) load_UVs(m, uvs);

		if (positions.size() != normals.size())
		{
			throw std::runtime_error("Positions and normals mismatch.");
		}

		if (uvs.empty())
		{
			typedef opengl::vertex_PN<GLfloat> vertex_type;
			
			std::vector<vertex_type> vertices;

			for (std::size_t i = 0; i < positions.size(); ++i)
			{
				vertices.emplace_back(positions[i], normals[i]);
			}
			set_data(vertices);
		}
		else
		{
			typedef opengl::vertex_PUN<GLfloat> vertex_type;
			
			std::vector<vertex_type> vertices;

			for (std::size_t i = 0; i < positions.size(); ++i)
			{
				vertices.emplace_back(positions[i], uvs[i], normals[i]);
			}
			set_data(vertices);
		}
	}

	mesh::mesh(FbxMesh* m, const GLenum usageHint)
	{
		set_data(m, usageHint);
	}

	mesh::mesh(mesh&& other)
	// : object<mesh, key_type>(std::move(other))
	: m_vao(std::move(other.m_vao))
	, m_vbo(std::move(other.m_vbo))
	// , m_ibo(std::move(other.m_ibo))
	, m_count(std::move(other.m_count))
	, m_type(std::move(other.m_type))
	{
		/* Empty on purpose. */
	}

	mesh& mesh::operator = (mesh&& other)
	{
		// object<mesh, key_type>::operator=(std::move(other));
		m_vao = std::move(other.m_vao);
		m_vbo = std::move(other.m_vbo);
		// m_ibo = std::move(other.m_ibo);
		m_count = std::move(other.m_count);
		m_type = std::move(other.m_type);
		return *this;
	}

	// mesh::mesh(const key_type& k) : object<mesh, key_type>(k)
	// {
	// 	construct_from_key();
	// }

	// mesh::mesh(key_type&& k) : object<mesh, key_type>(std::move(k))
	// {
	// 	construct_from_key();
	// }

	// mesh::key_type mesh::process(FbxMesh* m, FbxManager* manager)
	// {
	// 	int i, j, index, polygon_size;
	// 	bool success;
	// 	FbxVector4 position;
	// 	FbxVector4 fbx_normal;
	// 	FbxVector2 fbx_uv;

	// 	if (m->IsTriangleMesh() == false)
	// 	{
	// 		throw std::runtime_error("Mesh is not fully triangulated.");
	// 	}

	// 	std::vector<GLuint> indices(m->GetPolygonVertices(), 
	// 		m->GetPolygonVertices() + m->GetPolygonVertexCount());
	// 	std::vector<vec3f> positions;

	// 	std::vector<vec3f> normals;
	// 	std::vector<vec2f> uvs;

	// 	const int polygon_count = m->GetPolygonCount();

	// 	FbxStringList uv_names;
	// 	m->GetUVSetNames(uv_names);
	// 	for (i = 0; i < polygon_count; ++i)
	// 	{
	// 		polygon_size = m->GetPolygonSize(i);
	// 		for (j = 0; j < polygon_size; ++j)
	// 		{
	// 			index = m->GetPolygonVertex(i, j);
	// 			if (index < 0)
	// 			{
	// 				throw std::runtime_error("Invalid polyon vertex index.");
	// 			}
	// 			else
	// 			{
	// 				positions.emplace_back(
	// 					m->GetControlPointAt(m->GetPolygonVertex(i,j)));
	// 				success = m->GetPolygonVertexNormal(i, j, fbx_normal);
	// 				if (!success)
	// 				{
	// 					throw std::runtime_error(
	// 						"Failed to load normal vector.");
	// 				}
	// 				else
	// 				{
	// 					normals.emplace_back(fbx_normal);
	// 				}
	// 				// if (uv_names.GetCount() > 0)
	// 				// {
	// 				// 	success = m->GetPolygonVertexUV(i, j, 
	// 				// 		uv_names[0], fbx_uv, unmapped);
	// 				// 	if (unmapped)
	// 				// 	{
	// 				// 		throw std::runtime_error("UV is unmapped.");
	// 				// 	}
	// 				// 	else
	// 				// 	{
	// 				// 		if (!success)
	// 				// 		{
	// 				// 			throw std::runtime_error(
	// 				// 				"Failed to load UV.");
	// 				// 		}
	// 				// 		else
	// 				// 		{
	// 				// 			uvs.emplace_back(fbx_uv[0], fbx_uv[1]);
	// 				// 		}
	// 				// 	}
	// 				// }
	// 			}
	// 		}
	// 	}

	// 	if (uv_names.GetCount() > 0) load_UVs(m, uvs);

	// 	if (positions.size() != normals.size())
	// 	{
	// 		throw std::runtime_error("Positions and normals mismatch.");
	// 	}

	// 	boost::filesystem::path filename("../resources");

	// 	if (uvs.empty())
	// 	{
	// 		typedef opengl::vertex_PN<GLfloat> vertex_type;
			
	// 		std::vector<vertex_type> vertices;
			
	// 		filename /= std::string(m->GetNode()->GetName()) 
	// 			+ std::string(vertex_type::extension());

	// 		for (std::size_t i = 0; i < positions.size(); ++i)
	// 		{
	// 			vertices.emplace_back(positions[i], normals[i]);
	// 		}
	// 		std::ofstream output(filename.c_str());
	// 		eos::portable_oarchive oa(output);
	// 		oa & vertices;
	// 	}
	// 	else
	// 	{
	// 		typedef opengl::vertex_PUN<GLfloat> vertex_type;
			
	// 		std::vector<vertex_type> vertices;

	// 		filename /= std::string(m->GetNode()->GetName()) 
	// 			+ std::string(vertex_type::extension());

	// 		for (std::size_t i = 0; i < positions.size(); ++i)
	// 		{
	// 			vertices.emplace_back(positions[i], uvs[i], normals[i]);
	// 		}
	// 		std::ofstream output(filename.c_str());
	// 		eos::portable_oarchive oa(output);
	// 		oa & vertices;
	// 	}
	// 	return filename;
	// }

	// bool mesh::validate(std::ostream& log, FbxMesh* m, FbxManager* manager)
	// {
	// 	int i, j, index, polygon_size;
	// 	bool success, unmapped;
	// 	FbxVector4 position;
	// 	FbxVector4 fbx_normal;
	// 	FbxVector2 fbx_uv;

	// 	if (m->IsTriangleMesh() == false)
	// 	{
	// 		log << "Mesh \"" << m->GetNode()->GetName() 
	// 			<< "\": is not fully triangulated.\n";
	// 		return false;
	// 	}

	// 	std::vector<GLuint> indices(m->GetPolygonVertices(), 
	// 		m->GetPolygonVertices() + m->GetPolygonVertexCount());
	// 	std::vector<vec3f> positions;

	// 	std::vector<vec3f> normals;
	// 	std::vector<vec2f> uvs;

	// 	const int polygon_count = m->GetPolygonCount();

	// 	FbxStringList uv_names;
	// 	m->GetUVSetNames(uv_names);
	// 	log << "Mesh \"" << m->GetNode()->GetName() << "\": UV sets:\n"; 
	// 	for (i = 0; i < uv_names.GetCount(); ++i)
	// 	{
	// 		log << '\t' << uv_names[i] << '\n';
	// 	}

	// 	for (i = 0; i < polygon_count; ++i)
	// 	{
	// 		polygon_size = m->GetPolygonSize(i);
	// 		for (j = 0; j < polygon_size; ++j)
	// 		{
	// 			index = m->GetPolygonVertex(i, j);
	// 			if (index < 0)
	// 			{
	// 				log << "Mesh \"" << m->GetNode()->GetName() << "\": "
	// 					<< "Invalid polygon vertex index: " << i << ", " 
	// 					<< j << '\n';
	// 			}
	// 			else
	// 			{
	// 				positions.emplace_back(
	// 					m->GetControlPointAt(m->GetPolygonVertex(i,j)));
	// 				success = m->GetPolygonVertexNormal(i, j, fbx_normal);
	// 				if (!success)
	// 				{
	// 					log << "Mesh \"" << m->GetNode()->GetName() << "\": "
	// 						<< "Failed to load normal vector: " << i << ", " 
	// 						<< j << '\n';
	// 				}
	// 				else
	// 				{
	// 					normals.emplace_back(fbx_normal);
	// 				}
	// 				if (uv_names.GetCount() > 0)
	// 				{
	// 					success = m->GetPolygonVertexUV(i, j, 
	// 						uv_names[0], fbx_uv, unmapped);
	// 					if (unmapped)
	// 					{
	// 						log << "Mesh \"" << m->GetNode()->GetName() 
	// 							<< "\": UV at " << i << ", " << j 
	// 							<< " is unmapped.\n";
	// 					}
	// 					else
	// 					{
	// 						if (!success)
	// 						{
	// 							log << "Mesh \"" << m->GetNode()->GetName()
	// 								<< "\": Failed to load UV: " << i << ", " 
	// 								<< j << '\n';
	// 						}
	// 						else
	// 						{
	// 							uvs.emplace_back(fbx_uv[0], fbx_uv[1]);
	// 						}
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}

	// 	if (positions.size() != normals.size())
	// 	{
	// 		log << "Mesh \"" << m->GetNode()->GetName() << "\": " 
	// 			<< "Number of position vectors is not equal to the number of "
	// 			<< "normal vectors.\n";
	// 		return false;
	// 	}

	// 	if (uvs.empty())
	// 	{
	// 		log << "Mesh \"" << m->GetNode()->GetName() << "\": " 
	// 			<< "No UVs are present.\n";
	// 		typedef opengl::vertex_PN<GLfloat> vertex_type;
	// 		std::vector<vertex_type> vertices;
	// 		for (std::size_t i = 0; i < positions.size(); ++i)
	// 		{
	// 			vertices.emplace_back(positions[i], normals[i]);
	// 		}
	// 		log << "Mesh \"" << m->GetNode()->GetName() << "\": " 
	// 			<< vertices.size() << " vertices.\n"
	// 			<< "Mesh \"" << m->GetNode()->GetName() << "\": "
	// 			<< indices.size() << " indices.\n"
	// 			<< "Mesh \"" << m->GetNode()->GetName() << "\": " 
	// 			<< "Seems to be OK.\n";
	// 		return true;
	// 	}
	// 	else
	// 	{
	// 		typedef opengl::vertex_PUN<GLfloat> vertex_type;
	// 		std::vector<vertex_type> vertices;
	// 		for (std::size_t i = 0; i < positions.size(); ++i)
	// 		{
	// 			vertices.emplace_back(positions[i], uvs[i], normals[i]);
	// 		}
	// 		log << "Mesh \"" << m->GetNode()->GetName() << "\": " 
	// 			<< vertices.size() << " vertices.\n"
	// 			<< "Mesh \"" << m->GetNode()->GetName() << "\": "
	// 			<< indices.size() << " indices.\n"
	// 			<< "Mesh \"" << m->GetNode()->GetName() << "\": " 
	// 			<< "Seems to be OK.\n";
	// 		return true;
	// 	}
	// }

	void mesh::draw() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glDrawArrays(GL_TRIANGLES, 0, m_count);
		// glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, 0);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}

	const char* mesh::type() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_type;
	}

	// void mesh::construct_from_key()
	// {
	// 	std::ifstream input(key().c_str(), std::ios::binary);
	// 	eos::portable_iarchive ia(input);
	// 	const auto ext = key().extension();
	// 	// std::vector<GLuint> indices;
	// 	if (ext == opengl::vertex_P<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_P<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PC<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PC<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PU<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PU<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PN<GLfloat>::extension())
	// 	{

	// 		std::vector<opengl::vertex_PN<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PUN<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PUN<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PCU<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PCU<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PUNTB<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PUNTB<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PCUNTB<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PCUNTB<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else if (ext == opengl::vertex_PCUN<GLfloat>::extension())
	// 	{
	// 		std::vector<opengl::vertex_PCUN<GLfloat>> vertices;
	// 		ia & vertices;
	// 		// ia & indices;
	// 		set_data(vertices/*, indices*/);
	// 	}
	// 	else
	// 	{
	// 		BOOST_THROW_EXCEPTION(unknown_vertex_format() 
	// 			<< errinfo_path(key()));
	// 	}
	// }
} // namespace gintonic