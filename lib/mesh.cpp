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

	bool mesh::vec2f::operator==(const mesh::vec2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == other.x && y == other.y;
	}

	bool mesh::vec2f::operator!=(const mesh::vec2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	void mesh::vec2f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(mesh::vec2f), nullptr);
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

	bool mesh::vec3f::operator==(const mesh::vec3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool mesh::vec3f::operator!=(const mesh::vec3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	void mesh::vec3f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(mesh::vec3f), nullptr);
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

	bool mesh::vec4f::operator==(const mesh::vec4f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool mesh::vec4f::operator!=(const mesh::vec4f& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}

	void mesh::vec4f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(mesh::vec4f), nullptr);
	}

	void mesh::set_data(FbxMesh* m, const GLenum usagehint)
	{
		int i, j, polyvertex, polygonsize, polygoncount, vertexid = 0;

		if (m->IsTriangleMesh() == false)
		{
			exception e(m->GetNode()->GetName());
			e.append(": not fully triangulated.");
			throw e;
		}

		std::vector<GLuint> indices;
		std::vector<mesh::vec4f> positions;
		std::vector<mesh::vec4f> uvs;
		std::vector<mesh::vec4f> normals;

		mesh::vec4f position;
		mesh::vec4f texcoord;
		mesh::vec4f normal(0.0f, 0.0f, 0.0f, 1.0f);

		FbxVector4 fbx_position;
		FbxVector2 fbx_texcoord;
		FbxVector4 fbx_normal;
		FbxVector4 fbx_tangent;
		FbxVector4 fbx_bitangent;

		gintonic::vec3f N, T, B;

		GLfloat handedness;

		bool has_tangents = false;

		// FBX calls the bitangent vectors binormals... sigh.
		FbxGeometryElementUV* fbx_uvs = nullptr;
		FbxGeometryElementNormal* fbx_normals = nullptr;
		FbxGeometryElementTangent* fbx_tangents = nullptr;
		FbxGeometryElementBinormal* fbx_binormals = nullptr;

		FbxStringList uvsetnames;
		m->GetUVSetNames(uvsetnames);

		if (uvsetnames.GetCount()) 
			fbx_uvs = m->GetElementUV(uvsetnames.GetStringAt(0));
		if (m->GetElementNormalCount()) 
			fbx_normals = m->GetElementNormal(0);
		if (m->GetElementTangentCount()) 
			fbx_tangents = m->GetElementTangent(0);
		if (m->GetElementBinormalCount()) 
			fbx_binormals = m->GetElementBinormal(0);

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
					N[0] = N[1] = N[2] = 0.0f;
				}
				else
				{
					N = normalize(gintonic::vec3f(fbx_normal[0], fbx_normal[1], fbx_normal[2]));
				}
				if (!get_element(fbx_uvs, polyvertex, vertexid, fbx_texcoord))
				{
					fbx_texcoord[0] = fbx_texcoord[1] = 0.0f;
				}
				if (!get_element(fbx_tangents, polyvertex, vertexid, fbx_tangent))
				{
					T[0] = T[1] = T[2] = 0.0f;
					has_tangents = false;
				}
				else
				{
					T = normalize(gintonic::vec3f(
						static_cast<GLfloat>(fbx_tangent[0]), 
						static_cast<GLfloat>(fbx_tangent[1]),
						static_cast<GLfloat>(fbx_tangent[2])));
					has_tangents = true;
				}
				if (!get_element(fbx_binormals, polyvertex, vertexid, fbx_bitangent))
				{
					B[0] = B[1] = B[2] = 0.0f;
				}
				else
				{
					B = normalize(gintonic::vec3f(
						static_cast<GLfloat>(fbx_bitangent[0]),
						static_cast<GLfloat>(fbx_bitangent[1]),
						static_cast<GLfloat>(fbx_bitangent[2])));
				}

				handedness = distance(N % T, B) < 0.01f ? 1.0f : -1.0f;

				position =
				{
					static_cast<GLfloat>(fbx_position[0]),
					static_cast<GLfloat>(fbx_position[1]),
					static_cast<GLfloat>(fbx_position[2]),
					N[0]
				};

				texcoord =
				{
					static_cast<GLfloat>(fbx_texcoord[0]),
					static_cast<GLfloat>(fbx_texcoord[1]),
					N[1],
					N[2]
				};

				if (has_tangents) normal = {T[0], T[1], T[2], handedness};

				// At this point, we could simply add
				// the position, texcoord and optionally normal
				// to their containers. However, we are going
				// to search through all containers and see
				// if there is already a vertex that is equal
				// to the current one. If there is, we insert
				// its index into the indices array.
				// Otherwise (if there is no duplicate vertex),
				// we insert the new position, texcoord and
				// optionally normal to their arrays and
				// insert a new index at the end of the indices
				// array (and increase the index counter).
				
				bool found_duplicate = false;
				GLuint index;
				for (index = 0; index < positions.size(); ++index)
				{
					if (positions[index] == position && uvs[index] == texcoord)
					{
						if (has_tangents)
						{
							if (normals[index] == normal)
							{
								found_duplicate = true;
								break;
							}
						}
						else
						{
							found_duplicate = true;
							break;
						}
					}
				}
				if (!found_duplicate)
				{
					positions.push_back(position);
					uvs.emplace_back(texcoord);
					if (has_tangents) normals.push_back(normal);
				}
				indices.push_back(index);
				++vertexid;
			}
		}
		
		std::cerr << "\tNum indices: " << indices.size() << '\n';
		std::cerr << "\tNum positions: " << positions.size() << '\n';
		std::cerr << "\tNum UVs: " << uvs.size() << '\n';
		std::cerr << "\tNum normals: " << normals.size() << '\n';

		// Enable the Vertex Array Object.
		glBindVertexArray(m_vao);

		// Upload the (packed) vertex data in separate buffers to the GPU.
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_POSITION]);
		gtBufferData(GL_ARRAY_BUFFER, positions, usagehint);
		mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_TEXCOORD]);
		gtBufferData(GL_ARRAY_BUFFER, uvs, usagehint);
		mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_TEXCOORD);

		if (has_tangents)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_NORMAL]);
			gtBufferData(GL_ARRAY_BUFFER, normals, usagehint);
			mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_NORMAL);
		}

		// Set up the matrix attributes for instanced rendering.
		// glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_PVM_MATRIX]);
		// for (GLuint i = 0; i < 4; ++i)
		// {
		// 	glEnableVertexAttribArray(GT_VERTEX_LAYOUT_PVM_MATRIX + i);
		// 	glVertexAttribPointer(GT_VERTEX_LAYOUT_PVM_MATRIX + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (const GLvoid*)(sizeof(GLfloat) * i * 4));
		// 	glVertexAttribDivisor(GT_VERTEX_LAYOUT_PVM_MATRIX + i, 1);
		// }
		// glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_VM_MATRIX]);
		// for (GLuint i = 0; i < 3; ++i)
		// {
		// 	glEnableVertexAttribArray(GT_VERTEX_LAYOUT_VM_MATRIX + i);
		// 	glVertexAttribPointer(GT_VERTEX_LAYOUT_VM_MATRIX + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 4, (const GLvoid*)(sizeof(GLfloat) * i * 4));
		// 	glVertexAttribDivisor(GT_VERTEX_LAYOUT_VM_MATRIX + i, 1);
		// }
		// glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_N_MATRIX]);
		// for (GLuint i = 0; i < 3; ++i)
		// {
		// 	glEnableVertexAttribArray(GT_VERTEX_LAYOUT_N_MATRIX + i);
		// 	glVertexAttribPointer(GT_VERTEX_LAYOUT_N_MATRIX + i, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (const GLvoid*)(sizeof(GLfloat) * i * 3));
		// 	glVertexAttribDivisor(GT_VERTEX_LAYOUT_N_MATRIX + i, 1);
		// }

		// Upload the indices for the arrays to the GPU and remember the count.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer[GT_MESH_INDEX]);
		gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, usagehint);
		m_count = static_cast<GLsizei>(indices.size());
	}

	mesh::mesh(FbxMesh* m, const GLenum usagehint)
	{
		set_data(m, usagehint);
	}

	void mesh::draw() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, nullptr);
		// glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_PVM_MATRIX]);
		// glBindBuffer(GL_ARRAY_BUFFER, sizeof(mat4f), renderer::matrix_PVM(), GL_DYNAMIC_DRAW);
		// glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_VM_MATRIX]);
		// glBindBuffer(GL_ARRAY_BUFFER, sizeof(mat4f), renderer::matrix_VM(), GL_DYNAMIC_DRAW);
		// glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_N_MATRIX]);
		// gtBufferData(GL_ARRAY_BUFFER, sizeof(mat4f), renderer::matrix_N(), GL_DYNAMIC_DRAW);
		// glDrawElementsInstanced(
		// 	GL_TRIANGLES, 
		// 	m_count,
		// 	GL_UNSIGNED_INT,
		// 	nullptr, 
		// 	1);
	}

	void mesh::draw(
		const std::vector<mat4f>& PVM_matrices,
		const std::vector<mat4f>& VM_matrices,
		const std::vector<mat3f>& N_matrices)
		const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_PVM_MATRIX]);
		gtBufferData(GL_ARRAY_BUFFER, PVM_matrices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_VM_MATRIX]);
		gtBufferData(GL_ARRAY_BUFFER, VM_matrices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer[GT_MESH_N_MATRIX]);
		gtBufferData(GL_ARRAY_BUFFER, N_matrices, GL_DYNAMIC_DRAW);
		glDrawElementsInstanced(
			GL_TRIANGLES, 
			m_count,
			GL_UNSIGNED_INT,
			nullptr, 
			static_cast<GLsizei>(PVM_matrices.size()));
	}

	unsigned mesh::type() const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return m_type;
	}

} // namespace gintonic