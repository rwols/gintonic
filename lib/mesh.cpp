#include "mesh.hpp"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "portable_oarchive.hpp"
#include "portable_iarchive.hpp"

#define GT_MESH_INDEX 0
#define GT_MESH_POSITION 1
#define GT_MESH_TEXCOORD 2
#define GT_MESH_NORMAL 3
#define GT_MESH_PVM_MATRIX 6
#define GT_MESH_VM_MATRIX 7
#define GT_MESH_N_MATRIX 8

namespace { // begin anonymous namespace

template <class LayerElement, class VectorType> 
bool get_element(
	const LayerElement* e, 
	const int polyvertex,
	const int vertexid,
	VectorType& r)
{
	if (!e) return false;
	switch(e->GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (e->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					r = e->GetDirectArray().GetAt(polyvertex);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					auto index2element = e->GetIndexArray().GetAt(polyvertex);
					r = e->GetDirectArray().GetAt(index2element);
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
					r = e->GetDirectArray().GetAt(vertexid);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					auto index2element = e->GetIndexArray().GetAt(vertexid);
					r = e->GetDirectArray().GetAt(index2element);
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
	return true;
}

} // anonymous namespace

namespace gintonic
{

	mesh::vec2f::vec2f(const FbxVector2& v)
	: x(static_cast<GLfloat>(v[0]))
	, y(static_cast<GLfloat>(v[1]))
	{
		/* Empty on purpose. */
	}

	mesh::vec2f::vec2f(const GLfloat x, const GLfloat y)
	: x(x)
	, y(y)
	{
		/* Empty on purpose. */
	}

	void mesh::vec2f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	mesh::vec3f::vec3f(const FbxVector4& v)
	: x(static_cast<GLfloat>(v[0]))
	, y(static_cast<GLfloat>(v[1]))
	, z(static_cast<GLfloat>(v[2]))
	{
		/* Empty on purpose. */
	}

	mesh::vec3f::vec3f(const GLfloat x, const GLfloat y, const GLfloat z)
	: x(x)
	, y(y)
	, z(z)
	{
		/* Empty on purpose. */
	}

	void mesh::vec3f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	mesh::vec4f::vec4f(const FbxVector4& v)
	: x(static_cast<GLfloat>(v[0]))
	, y(static_cast<GLfloat>(v[1]))
	, z(static_cast<GLfloat>(v[2]))
	, w(static_cast<GLfloat>(v[3]))
	{
		/* Empty on purpose. */
	}

	mesh::vec4f::vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
	{
		/* Empty on purpose. */
	}

	void mesh::vec4f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	void mesh::set_data(FbxMesh* m, const GLenum usagehint)
	{
		int i, j, polyvertex, polygonsize, polygoncount, vertexid = 0;

		if (m->IsTriangleMesh() == false)
		{
			throw std::runtime_error("mesh::mesh: not fully triangulated.");
		}

		std::vector<GLuint> indices(m->GetPolygonVertices(), 
			m->GetPolygonVertices() + m->GetPolygonVertexCount());
		
		std::vector<mesh::vec4f> positions;
		std::vector<mesh::vec4f> uvs;
		std::vector<mesh::vec4f> tangents;

		FbxVector4 fbx_position;
		FbxVector2 fbx_texcoord;
		FbxVector4 fbx_normal;
		FbxVector4 fbx_tangent;
		FbxVector4 fbx_bitangent;

		gintonic::vec3f N, T, B;

		GLfloat handedness;

		// FBX calls the bitangent vectors binormals... sigh.
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

		// For each triangle ...
		polygoncount = m->GetPolygonCount();
		for (i = 0; i < polygoncount; ++i)
		{
			polygonsize = m->GetPolygonSize(i);
			assert(polygonsize == 3);
			// For each vertex in the current triangle ...
			for (j = 0; j < polygonsize; ++j)
			{
				polyvertex = m->GetPolygonVertex(i, j);

				fbx_position = m->GetControlPointAt(polyvertex);

				if (!get_element(fbx_normals, polyvertex, vertexid, fbx_normal))
				{
					fbx_normal[0] = fbx_normal[1] = fbx_normal[2] = fbx_normal[2] = 0.0f;
				}
				if (!get_element(fbx_uvs, polyvertex, vertexid, fbx_texcoord))
				{
					fbx_texcoord[0] = fbx_texcoord[1] = 0.0f;
				}
				if (!get_element(fbx_tangents, polyvertex, vertexid, fbx_tangent)
				{
					fbx_tangent[0] = fbx_tangent[1] = fbx_tangent[2] = fbx_tangent[2] = 0.0f;
				}
				if (!get_element(fbx_binormals, polyvertex, vertexid, fbx_bitangent)
				{
					fbx_bitangent[0] = fbx_bitangent[1] = fbx_bitangent[2] = fbx_bitangent[2] = 0.0f;
				}

				N = normalize(gintonic::vec3f(fbx_normal[0], fbx_normal[1], fbx_normal[2]));
				T = normalize(gintonic::vec3f(fbx_tangent[0], fbx_tangent[1], fbx_tangent[2]));
				B = normalize(gintonic::vec3f(fbx_bitangent[0], fbx_bitangent[1], fbx_bitangent[2]));

				handedness = distance(cross(N,T),B) < 0.01f ? 1.0f : -1.0f;

				positions.emplace_back(fbx_position[0], fbx_position[1], fbx_position[2], N[0]);
				uvs.emplace_back(fbx_texcoord[0], fbx_texcoord[1], N[1], N[2]);
				normals.emplace_back(T[0], T[1], T[2], handedness);

				// In the vertex shader, to get the bitangent vector you do:
				//
				// vec3 N(in_position.w, in_texcoord.y, in_texcoord.w);
				// vec3 T(in_normal.x, in_normal.y, in_normal.z);
				// vec3 B = in_normal.w * cross(N,T);

				++vertexid;
			}
		}

		// Enable the Vertex Array Object.
		glBindVertexArray(m_vao);

		// Upload the indices for the arrays to the GPU and remember the count.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer[GT_MESH_INDEX]);
		gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, usagehint);
		m_count = static_cast<GLsizei>(indices.size());

		// Upload the (packed) vertex data in separate buffers to the GPU.
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_POSITION]);
		gtBufferData(GL_ARRAY_BUFFER, positions, usagehint);
		mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_TEXCOORD]);
		gtBufferData(GL_ARRAY_BUFFER, uvs, usagehint);
		mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_TEXCOORD);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_NORMAL]);
		gtBufferData(GL_ARRAY_BUFFER, normals, usagehint);
		mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_NORMAL);

		// Set up the matrix attributes for instanced rendering.
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_PVM_MATRIX]);
		for (GLuint i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(GT_VERTEX_LAYOUT_PVM_MATRIX + i);
			glVertexAttribPointer(GT_VERTEX_LAYOUT_PVM_MATRIX + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (const GLvoid*)(sizeof(GLfloat) * i * 4));
			glVertexAttribDivisor(GT_VERTEX_LAYOUT_PVM_MATRIX + i, 1);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_VM_MATRIX]);
		for (GLuint i = 0; i < 3; ++i)
		{
			glEnableVertexAttribArray(GT_VERTEX_LAYOUT_VM_MATRIX + i);
			glVertexAttribPointer(GT_VERTEX_LAYOUT_VM_MATRIX + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 4, (const GLvoid*)(sizeof(GLfloat) * i * 4));
			glVertexAttribDivisor(GT_VERTEX_LAYOUT_VM_MATRIX + i, 1);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_N_MATRIX])
		for (GLuint i = 0; i < 3; ++i)
		{
			glEnableVertexAttribArray(GT_VERTEX_LAYOUT_N_MATRIX + i);
			glVertexAttribPointer(GT_VERTEX_LAYOUT_N_MATRIX + i, 3, GL_FLOAT, GL_FALSE, sizeof(GLFloat) * 3 * 3, (const GLvoid*)(sizeof(GLfloat) * i * 3));
			glVertexAttribDivisor(GT_VERTEX_LAYOUT_N_MATRIX + i, 1);
		}

		// if (uvs.empty())
		// {
		// 	if (tangents.empty() && bitangents.empty())
		// 	{
		// 		typedef opengl::vertex_PN<GLfloat> vertex_type;
		// 		std::cerr << "Vertex type is " << vertex_type::extension() << '\n';
		// 		// std::vector<vertex_type> vertices;

		// 		// for (std::size_t i = 0; i < positions.size(); ++i)
		// 		// {
		// 		// 	vertices.emplace_back(positions[i], normals[i]);
		// 		// }
		// 		// set_data(vertices);

		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_POSITION]);
		// 		gtBufferData(GL_ARRAY_BUFFER, positions, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_POSITION);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_NORMAL]);
		// 		gtBufferData(GL_ARRAY_BUFFER, normals, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_NORMAL);
		// 	}
		// 	else
		// 	{
		// 		throw std::runtime_error("Vertex type not supported.");
		// 	}
		// }
		// else
		// {
		// 	if (tangents.empty() && bitangents.empty())
		// 	{
		// 		typedef opengl::vertex_PUN<GLfloat> vertex_type;
		// 		std::cerr << "Vertex type is " << vertex_type::extension() << '\n';
		// 		// std::vector<vertex_type> vertices;

		// 		// for (std::size_t i = 0; i < positions.size(); ++i)
		// 		// {
		// 		// 	vertices.emplace_back(positions[i], uvs[i], normals[i]);
		// 		// }
		// 		// set_data(vertices);

		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_POSITION]);
		// 		gtBufferData(GL_ARRAY_BUFFER, positions, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_POSITION);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_TEXCOORD]);
		// 		gtBufferData(GL_ARRAY_BUFFER, uvs, usagehint);
		// 		mesh::vec2f::enable_attribute(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_NORMAL]);
		// 		gtBufferData(GL_ARRAY_BUFFER, normals, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_NORMAL);
		// 	}
		// 	else
		// 	{
		// 		typedef opengl::vertex_PUNTB<GLfloat> vertex_type;
		// 		std::cerr << "Vertex type is " << vertex_type::extension() << '\n';
		// 		// std::vector<vertex_type> vertices;

		// 		// for (std::size_t i = 0; i < positions.size(); ++i)
		// 		// {
		// 		// 	vertices.emplace_back(positions[i], uvs[i], normals[i], tangents[i], bitangents[i]);
		// 		// }

		// 		// for (const auto& v : vertices)
		// 		// {
		// 		// 	std::cout << v << '\n';
		// 		// }

		// 		// set_data(vertices);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_POSITION]);
		// 		gtBufferData(GL_ARRAY_BUFFER, positions, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_POSITION);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_TEXCOORD]);
		// 		gtBufferData(GL_ARRAY_BUFFER, uvs, usagehint);
		// 		mesh::vec2f::enable_attribute(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_NORMAL]);
		// 		gtBufferData(GL_ARRAY_BUFFER, normals, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_NORMAL);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_TANGENT]);
		// 		gtBufferData(GL_ARRAY_BUFFER, tangents, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_TANGENT);
		// 		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_BITANGENT]);
		// 		gtBufferData(GL_ARRAY_BUFFER, bitangents, usagehint);
		// 		mesh::vec3f::enable_attribute(GINTONIC_VERTEX_LAYOUT_BITANGENT);
		// 	}

		// }
	}

	mesh::mesh(FbxMesh* m, const GLenum usagehint)
	{
		set_data(m, usagehint);
	}

	void mesh::draw() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, nullptr);
	}

	void mesh::draw(const std::vector<mat4f>& PVM_matrices) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);

	}

	unsigned mesh::type() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_type;
	}

} // namespace gintonic