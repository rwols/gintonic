#include "mesh.hpp"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

namespace { // begin anonymous namespace

template <class LayerElement, class VectorType> 
void get_element(
	const LayerElement* e, 
	const int polyvertex,
	const int vertexid,
	std::vector<VectorType>& r)
{
	if (!e) return;
	switch(e->GetMappingMode())
	{
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
			break;
		}
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch(e->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					auto element = e->GetDirectArray().GetAt(vertexid);
					r.push_back(element);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					auto index2element = e->GetIndexArray().GetAt(vertexid);
					auto element = e->GetDirectArray().GetAt(index2element);
					r.push_back(element);
					break;
				}
				default:
				{
					throw std::runtime_error("Reference mode not supported.");
				}
			}
			break;
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
		int i, j, polyvertex, polygonsize, polygoncount, vertexid = 0;
		
		// FbxVector4 position;
		// FbxVector4 fbx_normal;
		// FbxVector2 fbx_uv;

		if (m->IsTriangleMesh() == false)
		{
			throw std::runtime_error("mesh::mesh: not fully triangulated.");
		}

		// std::vector<GLuint> indices(m->GetPolygonVertices(), 
		// 	m->GetPolygonVertices() + m->GetPolygonVertexCount());
		
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

		std::cerr << "UV set name: " << uvsetnames.GetStringAt(0) << '\n';

		polygoncount = m->GetPolygonCount();
		for (i = 0; i < polygoncount; ++i)
		{
			polygonsize = m->GetPolygonSize(i);
			assert(polygonsize == 3);
			for (j = 0; j < polygonsize; ++j)
			{
				polyvertex = m->GetPolygonVertex(i, j);
				positions.emplace_back(m->GetControlPointAt(polyvertex));

				get_element(fbx_normals, polyvertex, vertexid, normals);
				get_element(fbx_uvs, polyvertex, vertexid, uvs);
				get_element(fbx_tangents, polyvertex, vertexid, tangents);
				get_element(fbx_binormals, polyvertex, vertexid, bitangents);

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
				std::cerr << "Vertex type is " << vertex_type::extension() << '\n';
				std::vector<vertex_type> vertices;

				for (std::size_t i = 0; i < positions.size(); ++i)
				{
					vertices.emplace_back(positions[i], normals[i]);
				}
				set_data(vertices);
			}
			else
			{
				throw std::runtime_error("Vertex type not supported.");
				// typedef opengl::vertex_PNTB<GLfloat> vertex_type;

				// std::vector<vertex_type> vertices;

				// for (std::size_t i = 0; i < positions.size(); ++i)
				// {
				// 	vertices.emplace_back(positions[i], normals[i], tangents[i], bitangents[i]);
				// }
				// set_data(vertices);
			}
		}
		else
		{
			if (tangents.empty() && bitangents.empty())
			{
				typedef opengl::vertex_PUN<GLfloat> vertex_type;
				std::cerr << "Vertex type is " << vertex_type::extension() << '\n';
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
				std::cerr << "Vertex type is " << vertex_type::extension() << '\n';
				std::vector<vertex_type> vertices;

				for (std::size_t i = 0; i < positions.size(); ++i)
				{
					vertices.emplace_back(positions[i], uvs[i], normals[i], tangents[i], bitangents[i]);
				}

				for (const auto& v : vertices)
				{
					std::cout << v << '\n';
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