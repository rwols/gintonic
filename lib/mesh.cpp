#include "mesh.hpp"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

namespace
{

// void load_UVs(FbxMesh* pMesh, std::vector<gintonic::vec2f>& uvs)
// {
// 	//get all UV set names
// 	FbxStringList lUVSetNameList;
// 	pMesh->GetUVSetNames(lUVSetNameList);
	
// 	// Assume index 0 is the correct one.
// 	const char* lUVSetName = lUVSetNameList.GetStringAt(0);
// 	const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

// 	if(!lUVElement) throw std::runtime_error("Could not fetch geometry element UV.");

// 	// only support mapping mode eByPolygonVertex and eByControlPoint
// 	if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
// 		lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
// 	{
// 		throw std::runtime_error("Mapping mode of UVs not supported.\n");		
// 	}

// 	//index array, where holds the index referenced to the uv data
// 	const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
// 	const int lIndexCount= (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

// 	//iterating through the data by polygon
// 	const int lPolyCount = pMesh->GetPolygonCount();

// 	if(lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
// 	{
// 		for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
// 		{
// 			// build the max index array that we need to pass into MakePoly
// 			const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
// 			for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
// 			{
// 				FbxVector2 lUVValue;

// 				//get the index of the current vertex in control points array
// 				int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex,lVertIndex);

// 				//the UV index depends on the reference mode
// 				int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

// 				lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

// 				//User TODO:
// 				//Print out the value of UV(lUVValue) or log it to a file
// 				uvs.emplace_back(static_cast<float>(lUVValue[0]), static_cast<float>(lUVValue[1]));
// 			}
// 		}
// 	}
// 	else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
// 	{
// 		int lPolyIndexCounter = 0;
// 		for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
// 		{
// 			// build the max index array that we need to pass into MakePoly
// 			const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
// 			for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
// 			{
// 				if (lPolyIndexCounter < lIndexCount)
// 				{
// 					FbxVector2 lUVValue;

// 					//the UV index depends on the reference mode
// 					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

// 					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

// 					//User TODO:
// 					//Print out the value of UV(lUVValue) or log it to a file
// 					uvs.emplace_back(static_cast<float>(lUVValue[0]), static_cast<float>(lUVValue[1]));

// 					lPolyIndexCounter++;
// 				}
// 			}
// 		}
// 	}
// }

// vec3f read_normal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter)
// {
// 	if(inMesh->GetElementNormalCount() < 1)
// 	{
// 		throw std::exception("Invalid Normal Number");
// 	}

// 	vec3f outNormal;

// 	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
// 	switch(vertexNormal->GetMappingMode())
// 	{
// 	case FbxGeometryElement::eByControlPoint:
// 		switch(vertexNormal->GetReferenceMode())
// 		{
// 		case FbxGeometryElement::eDirect:
// 		{
// 			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
// 			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
// 			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
// 		}
// 		break;

// 		case FbxGeometryElement::eIndexToDirect:
// 		{
// 			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
// 			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
// 			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
// 			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
// 		}
// 		break;

// 		default:
// 			throw std::exception("Invalid Reference");
// 		}
// 		break;

// 	case FbxGeometryElement::eByPolygonVertex:
// 		switch(vertexNormal->GetReferenceMode())
// 		{
// 		case FbxGeometryElement::eDirect:
// 		{
// 			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
// 			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
// 			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
// 		}
// 		break;

// 		case FbxGeometryElement::eIndexToDirect:
// 		{
// 			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
// 			outNormal[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
// 			outNormal[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
// 			outNormal[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
// 		}
// 		break;

// 		default:
// 			throw std::exception("Invalid Reference");
// 		}
// 		break;
// 	}
// 	return outNormal;
// }

// void load_uv(FbxGeometryElementUV* fbx_uvs, int i, int j, int index, std::vector<gintonic::vec2f>& uvs)
// {
// 	if (!fbx_uvs) return;
// 	FbxVector2 uv;
// 	switch (fbx_uvs->GetMappingMode())
// 	{
// 		case FbxGeometryElement::eByControlPoint:
// 		{
// 			switch (fbx_uvs->GetReferenceMode())
// 			{
// 				case FbxGeometryElement::eDirect:
// 				{
// 					uv = fbx_uvs->GetDirectArray().GetAt(index);

// 					break;
// 				}
// 				case FbxGeometryElement::eIndexToDirect:
// 				{
// 					int id = fbx_uvs->GetIndexArray().GetAt(index);
// 					uv = fbx_uvs->GetDirectArray().GetAt(id);
// 					uvs.emplace_back(static_cast<float>(uv[0]), static_cast<float>(uv[1])); 
// 					break;
// 				}
// 				default:
// 				{
// 					throw std::runtime_error("Reference mode for UVs not supported.");
// 				}
// 			}
// 		}
// 		case FbxGeometryElement::eByPolygonVertex:
// 		{
// 			int texture_uv_index = m->GetTextureUVIndex(i, j);
// 			switch (fbx_uvs->GetReferenceMode())
// 			{
// 				case FbxGeometryElement::eDirect:
// 				case FbxGeometryElement::eIndexToDirect:
// 				{
// 					uv = fbx_uvs->GetDirectArray().GetAt(texture_uv_index);
// 					uvs.emplace_back(static_cast<float>(uv[0]), static_cast<float>(uv[1]));
// 					break;
// 				}
// 				default:
// 				{
// 					throw std::runtime_error("Reference mode for UVs not supported.");
// 				}
// 			}
// 			break;
// 		}
// 		default:
// 		{
// 			throw std::runtime_error("Mapping mode for UVs not supported.");
// 		}
// 	}
// }

// void load_normal(FbxGeometryElementNormal* fbx_normals, int i, int j, int index, std::vector<gintonic::vec2f>& uvs)
// {
// 	if (!fbx_normals) return;
// 	FbxVector4 normal;
// 	switch (fbx_normals->GetMappingMode())
// 	{
// 		case FbxGeometryElement::eByControlPoint:
// 		{
// 			switch (fbx_uvs->GetReferenceMode())
// 			{
// 				case FbxGeometryElement::eDirect:
// 				{
// 					uv = fbx_uvs->GetDirectArray().GetAt(index);
// 					break;
// 				}
// 				case FbxGeometryElement::eIndexToDirect:
// 				{
// 					int id = fbx_uvs->GetIndexArray().GetAt(index);
// 					uv = fbx_uvs->GetDirectArray().GetAt(id);
// 					uvs.emplace_back(static_cast<float>(uv[0]), static_cast<float>(uv[1])); 
// 					break;
// 				}
// 				default:
// 				{
// 					throw std::runtime_error("Reference mode for UVs not supported.");
// 				}
// 			}
// 		}
// 		case FbxGeometryElement::eByPolygonVertex:
// 		{
// 			int texture_uv_index = m->GetTextureUVIndex(i, j);
// 			switch (fbx_uvs->GetReferenceMode())
// 			{
// 				case FbxGeometryElement::eDirect:
// 				case FbxGeometryElement::eIndexToDirect:
// 				{
// 					uv = fbx_uvs->GetDirectArray().GetAt(texture_uv_index);
// 					uvs.emplace_back(static_cast<float>(uv[0]), static_cast<float>(uv[1]));
// 					break;
// 				}
// 				default:
// 				{
// 					throw std::runtime_error("Reference mode for UVs not supported.");
// 				}
// 			}
// 			break;
// 		}
// 		default:
// 		{
// 			throw std::runtime_error("Mapping mode for UVs not supported.");
// 		}
// 	}
// }

template <class LayerElement, class ReturnType> void get_element(
	const LayerElement* e, 
	const int i, 
	const int j, 
	const int vertexid,
	std::vector<ReturnType>& r)
{
	if (!e) return;
	switch(e->GetMappingMode())
	{
		auto polyvertex = m->GetPolygonVertex(i, j);
		case FbxGeometryElement::eByControlPoint:
		{
			switch (e->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					auto element = e->GetDirectArray().GetAt(polyvertex);
					r.push_back(element);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					auto index2element = e->GetIndexArray().GetAt(polyvertex);
					auto element = e->GetDirectArray().GetAt(index2element);
					r.push_back(element);
					break;
				}
				default:
				{
					throw std::runtime_error("Reference mode not supported.");
				}
			}
		}
		default:
		{
			throw std::runtime_error("Mapping mode not supported.");
		}
	}
}

} // anonymous namespace

namespace gintonic
{

	mesh::mesh() : m_count(0) {}

	void mesh::set_data(FbxMesh* m, const GLenum usageHint)
	{
		int i, j, index, polygon_size, polygon_count, vertexid = 0;
		
		// FbxVector4 position;
		// FbxVector4 fbx_normal;
		// FbxVector2 fbx_uv;

		if (m->IsTriangleMesh() == false)
		{
			throw std::runtime_error("mesh::mesh: not fully triangulated.");
		}

		std::vector<GLuint> indices(m->GetPolygonVertices(), 
			m->GetPolygonVertices() + m->GetPolygonVertexCount());
		
		std::vector<vec3f> positions;
		std::vector<vec2f> uvs;
		std::vector<vec3f> normals;
		std::vector<vec3f> tangents;
		std::vector<vec3f> bitangents;

		// FBX calls the bitangent vectors binormals.. sigh.
		FbxGeometryElementUV* fbx_uvs = nullptr;
		FbxGeometryElementNormal* fbx_normals = nullptr;
		FbxGeometryElementTangent* fbx_tangents = nullptr;
		FbxGeometryElementBinormal* fbx_binormals = nullptr;

		FbxStringList uvsetnames;
		m->GetUVSetNames(uvsetnames);

		if (uvsetnames.GetCount()) fbx_uvs = m->GetElementUV(uvsetnames.GetStringAt(0));
		if (m->GetElementNormalCount()) fbx_normals = m->GetElementNormal(0);
		if (m->GetElementTangentCount()) fbx_tangents = m->GetElementTangent(0);
		if (m->GetElementBinormalCount()) fbx_binormals = m->GetElementBinormal(0);

		polygon_count = m->GetPolygonCount();
		for (i = 0; i < polygon_count; ++i)
		{
			polygon_size = m->GetPolygonSize(i);
			assert(polygon_size == 3);
			for (j = 0; j < polygon_size; ++j)

				index = m->GetPolygonVertex(i, j);
				positions.emplace_back(m->GetControlPointAt(index));

				get_element(fbx_uvs, i, j, vertexid, uvs);
				get_element(fbx_normals, i, j, vertexid, normals);
				get_element(fbx_tangents, i, j, vertexid, tangents);
				get_element(fbx_binormals, i, j, vertexid, bitangents);

				++vertexid;
			}
		}

		// FbxStringList uv_names;
		// m->GetUVSetNames(uv_names);
		// if (uv_names.GetCount() > 0) load_UVs(m, uvs);

		if (positions.size() != normals.size())
		{
			throw std::runtime_error("Positions and normals mismatch.");
		}

		if (uvs.empty())
		{
			if (tangents.empty() && bitangents.empty())
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
				typedef opengl::vertex_PNTB<GLfloat> vertex_type;

				std::vector<vertex_type> vertices;

				for (std::size_t i = 0; i < positions.size(); ++i)
				{
					vertices.emplace_back(positions[i], normals[i], tangents[i], bitangents[i]);
				}
				set_data(vertices);
			}
		}
		else
		{
			if (tangents.empty() && bitangents.empty())
			{
				typedef opengl::vertex_PUN<GLfloat> vertex_type;
				
				std::vector<vertex_type> vertices;

				for (std::size_t i = 0; i < positions.size(); ++i)
				{
					vertices.emplace_back(positions[i], uvs[i], normals[i]);
				}
				set_data(vertices);
			}
			else
			{
				typedef opengl::vertex_PUNTB<GLfloat> vertex_type;

				std::vector<vertex_type> vertices;

				for (std::size_t i = 0; i < positions.size(); ++i)
				{
					vertices.emplace_back(positions[i], uvs[i], normals[i], tangents[i], bitangents[i]);
				}
				set_data(vertices);
			}

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

	void mesh::draw() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glDrawArrays(GL_TRIANGLES, 0, m_count);
	}

	const char* mesh::type() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_type;
	}

} // namespace gintonic