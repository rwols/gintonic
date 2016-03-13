#include "mesh.hpp"
#include "../entity.hpp"
#include "../Foundation/exception.hpp"
#include <fbxsdk.h>
#include <assimp/mesh.h>

#define GT_MESH_BUFFER_POS_XYZ_UV_X 0     // Buffer for the positions and uv.x
#define GT_MESH_BUFFER_NOR_XYZ_UV_Y 1     // Buffer for the normals and uv.y
#define GT_MESH_BUFFER_TAN_XYZ_HAND 2     // Buffer for the tangents and handedness
#define GT_MESH_BUFFER_INDICES 3          // Buffer for the indices

#define GT_VERTEX_LAYOUT_SLOT_0 0         //  pos.X  pos.Y  pos.Z   uv.X
#define GT_VERTEX_LAYOUT_SLOT_1 1         //  nor.X  nor.Y  nor.Z   uv.Y
#define GT_VERTEX_LAYOUT_SLOT_2 2         //  tan.X  tan.Y  tan.Z   hand
#define GT_VERTEX_LAYOUT_SLOT_3_4_5_6 3   // PVM.00 PVM.01 PVM.02 PVM.03 <--- instanced rendering
                                          // PVM.10 PVM.11 PVM.12 PVM.13 <--- instanced rendering
                                          // PVM.20 PVM.21 PVM.22 PVM.23 <--- instanced rendering
                                          // PVM.30 PVM.31 PVM.32 PVM.33 <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_7_8_9_10 7  //  VM.00  VM.01  VM.02  VM.03 <--- instanced rendering
                                          //  VM.10  VM.11  VM.12  VM.13 <--- instanced rendering
                                          //  VM.20  VM.21  VM.22  VM.23 <--- instanced rendering
                                          //  VM.30  VM.31  VM.32  VM.33 <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_11_12_13 11 //   N.00   N.01   N.02 unused <--- instanced rendering
                                          //   N.10   N.11   N.12 unused <--- instanced rendering
                                          //   N.20   N.21   N.22 unused <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_14 14       //   free   free   free   free
#define GT_VERTEX_LAYOUT_SLOT_15 15       //   free   free   free   free


namespace // anonymous namespace
{

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


namespace gintonic {

Mesh::vec2f::vec2f(const FbxVector2& v)
: x(static_cast<GLfloat>(v[0]))
, y(static_cast<GLfloat>(v[1]))
{
	/* Empty on purpose. */
}

Mesh::vec2f::vec2f(const GLfloat x, const GLfloat y)
: x(x)
, y(y)
{
	/* Empty on purpose. */
}

bool Mesh::vec2f::operator==(const Mesh::vec2f& other) const noexcept
{
	return x == other.x && y == other.y;
}

bool Mesh::vec2f::operator!=(const Mesh::vec2f& other) const noexcept
{
	return !operator==(other);
}

void Mesh::vec2f::enable_attribute(const GLuint index) noexcept
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec2f), nullptr);
}

Mesh::vec3f::vec3f(const FbxVector4& v)
: x(static_cast<GLfloat>(v[0]))
, y(static_cast<GLfloat>(v[1]))
, z(static_cast<GLfloat>(v[2]))
{
	/* Empty on purpose. */
}

Mesh::vec3f::vec3f(const GLfloat x, const GLfloat y, const GLfloat z)
: x(x)
, y(y)
, z(z)
{
	/* Empty on purpose. */
}

bool Mesh::vec3f::operator==(const Mesh::vec3f& other) const noexcept
{
	return x == other.x && y == other.y && z == other.z;
}

bool Mesh::vec3f::operator!=(const Mesh::vec3f& other) const noexcept
{
	return !operator==(other);
}

void Mesh::vec3f::enable_attribute(const GLuint index) noexcept
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec3f), nullptr);
}

Mesh::vec4f::vec4f(const FbxVector4& v)
: x(static_cast<GLfloat>(v[0]))
, y(static_cast<GLfloat>(v[1]))
, z(static_cast<GLfloat>(v[2]))
, w(static_cast<GLfloat>(v[3]))
{
	/* Empty on purpose. */
}

Mesh::vec4f::vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
: x(x)
, y(y)
, z(z)
, w(w)
{
	/* Empty on purpose. */
}

bool Mesh::vec4f::operator==(const Mesh::vec4f& other) const noexcept
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Mesh::vec4f::operator!=(const Mesh::vec4f& other) const noexcept
{
	return !operator==(other);
}

void Mesh::vec4f::enable_attribute(const GLuint index) noexcept
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec4f), nullptr);
}

Mesh::Mesh()
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
, mHasTangentsAndBitangents(false)
, mNumIndices(0)
{
	setupInstancedRenderingMatrices();
}

Mesh::Mesh(const aiMesh* pAssimpMesh)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(pAssimpMesh);
}

Mesh::Mesh(const FbxMesh* pFbxMesh)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(pFbxMesh);
}

Mesh::Mesh(const boost::filesystem::path& filename)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(filename);
}

Mesh::Mesh(
	const std::vector<GLuint>& indices, 
	const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(indices, position_XYZ_uv_X, normal_XYZ_uv_Y);
}

Mesh::Mesh(
	const std::vector<GLuint>& indices, 
	const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y,
	const std::vector<Mesh::vec4f>& tangent_XYZ_handedness)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(indices, position_XYZ_uv_X, normal_XYZ_uv_Y, tangent_XYZ_handedness);
}

void Mesh::set(const aiMesh* pAssimpMesh)
{
	if (!pAssimpMesh->HasPositions())
	{
		exception lException("Mesh with name \"");
		lException.append(pAssimpMesh->mName.C_Str());
		lException.append("\" has no position information.");
		throw lException;
	}

	if (!pAssimpMesh->HasNormals())
	{
		exception lException("Mesh with name \"");
		lException.append(pAssimpMesh->mName.C_Str());
		lException.append("\" has no normal information.");
		throw lException;
	}

	if (pAssimpMesh->mNumUVComponents[0] != 2)
	{
		exception lException("Mesh with name \"");
		lException.append(pAssimpMesh->mName.C_Str());
		lException.append("\" does not have exactly two texture components in its first slot.");
		throw lException;
	}

	std::vector<Mesh::vec4f> lSlot0Array; // p.x p.y p.z tex.x
	std::vector<Mesh::vec4f> lSlot1Array; // n.x n.y n.z tex.y
	std::vector<Mesh::vec4f> lSlot2Array; // t.x t.y t.z handedness
	std::vector<GLuint> lIndexArray;

	Mesh::vec4f lSlot0Entry;
	Mesh::vec4f lSlot1Entry;
	Mesh::vec4f lSlot2Entry;

	for (unsigned int v = 0; v < pAssimpMesh->mNumVertices; ++v)
	{
		lSlot0Entry.x = pAssimpMesh->mVertices[v].x;
		lSlot0Entry.y = pAssimpMesh->mVertices[v].y;
		lSlot0Entry.z = pAssimpMesh->mVertices[v].z;
		lSlot0Entry.w = pAssimpMesh->mTextureCoords[0][v].x;

		lSlot1Entry.x = pAssimpMesh->mNormals[v].x;
		lSlot1Entry.y = pAssimpMesh->mNormals[v].y;
		lSlot1Entry.z = pAssimpMesh->mNormals[v].z;
		lSlot1Entry.w = pAssimpMesh->mTextureCoords[0][v].y;

		if (pAssimpMesh->HasTangentsAndBitangents())
		{			
			const gintonic::vec3f lNormal(pAssimpMesh->mNormals[v].x, pAssimpMesh->mNormals[v].y, pAssimpMesh->mNormals[v].z);
			const gintonic::vec3f lTangent(pAssimpMesh->mTangents[v].x, pAssimpMesh->mTangents[v].y, pAssimpMesh->mTangents[v].z);
			const gintonic::vec3f lBitangent(pAssimpMesh->mBitangents[v].x, pAssimpMesh->mBitangents[v].y, pAssimpMesh->mBitangents[v].z);
			const float lHandedness = gintonic::distance(cross(lNormal, lTangent), lBitangent) < 0.01f ? 1.0f : -1.0f;

			lSlot2Entry.x = lTangent.x;
			lSlot2Entry.y = lTangent.y;
			lSlot2Entry.z = lTangent.z;
			lSlot2Entry.w = lHandedness;
		}

		bool lFoundDuplicate = false;
		GLuint lIndex;
		for (lIndex = 0; lIndex < lSlot0Array.size(); ++lIndex)
		{
			if (lSlot0Array[lIndex] == lSlot0Entry && lSlot1Array[lIndex] == lSlot1Entry)
			{
				if (pAssimpMesh->HasTangentsAndBitangents())
				{
					if (lSlot2Array[lIndex] == lSlot2Entry)
					{
						lFoundDuplicate = true;
						break;
					}
				}
				else
				{
					lFoundDuplicate = true;
					break;
				}
			}
		}
		if (!lFoundDuplicate)
		{
			lSlot0Array.push_back(lSlot0Entry);
			lSlot1Array.push_back(lSlot1Entry);
			if (pAssimpMesh->HasTangentsAndBitangents())
			{
				lSlot2Array.push_back(lSlot2Entry);
			}
		}
		lIndexArray.push_back(lIndex);
	}

	if (pAssimpMesh->HasTangentsAndBitangents())
	{
		set(lIndexArray, lSlot0Array, lSlot1Array, lSlot2Array);
		std::cout << "\tNumber of indices: " << lIndexArray.size() << '\n'
			<< "\tNumber of vertices: " << lSlot0Array.size() << '\n'
			<< "\tThis mesh has " << (mHasTangentsAndBitangents ? "no tangents.\n" : "tangents.\n");
	}
	else
	{
		set(lIndexArray, lSlot0Array, lSlot1Array);
	}
}

void Mesh::set(const FbxMesh* pFbxMesh)
{
	setNameWithFbx(pFbxMesh);

	int i, j, polyvertex, polygonsize, polygoncount, vertexid = 0;

	if (pFbxMesh->IsTriangleMesh() == false)
	{
		exception e(pFbxMesh->GetNode()->GetName());
		e.append(": not fully triangulated.");
		throw e;
	}

	std::vector<GLuint> lIndexArray;
	std::vector<Mesh::vec4f> lSlot0Array;
	std::vector<Mesh::vec4f> lSlot1Array;
	std::vector<Mesh::vec4f> lSlot2Array;

	gintonic::Mesh::vec4f lSlot0Entry;
	gintonic::Mesh::vec4f lSlot1Entry;
	gintonic::Mesh::vec4f lSlot2Entry{0.0f, 0.0f, 0.0f, 1.0f};

	FbxVector4 lFbxPosition;
	FbxVector2 lFbxTexCoord;
	FbxVector4 lFbxNormal;
	FbxVector4 lFbxTangent;
	FbxVector4 lFbxBitangent;

	gintonic::vec3f N;
	gintonic::vec3f T;
	gintonic::vec3f B;

	GLfloat handedness;

	// FBX calls the bitangent vectors binormals... sigh.
	const FbxGeometryElementUV* lFbxTexCoordArray = nullptr;
	const FbxGeometryElementNormal* lFbxNormalArray = nullptr;
	const FbxGeometryElementTangent* lFbxTangentArray = nullptr;
	const FbxGeometryElementBinormal* lFbxBitangentArray = nullptr;

	FbxStringList lUVSetNames;
	pFbxMesh->GetUVSetNames(lUVSetNames);

	if (lUVSetNames.GetCount()) 
	{
		lFbxTexCoordArray = pFbxMesh->GetElementUV(lUVSetNames.GetStringAt(0));
	}	
	if (pFbxMesh->GetElementNormalCount()) 
	{
		lFbxNormalArray = pFbxMesh->GetElementNormal(0);
	}
	if (pFbxMesh->GetElementTangentCount())
	{
		lFbxTangentArray = pFbxMesh->GetElementTangent(0);
	}
	if (pFbxMesh->GetElementBinormalCount()) 
	{
		lFbxBitangentArray = pFbxMesh->GetElementBinormal(0);
	}

	// For each triangle ...
	polygoncount = pFbxMesh->GetPolygonCount();
	for (i = 0; i < polygoncount; ++i)
	{
		polygonsize = pFbxMesh->GetPolygonSize(i);
		assert(polygonsize == 3);
		// For each vertex in the current triangle ...
		for (j = 0; j < polygonsize; ++j)
		{
			polyvertex = pFbxMesh->GetPolygonVertex(i, j);

			lFbxPosition = pFbxMesh->GetControlPointAt(polyvertex);

			if (!get_element(lFbxNormalArray, polyvertex, vertexid, lFbxNormal))
			{
				N = 0.0f;
			}
			else
			{
				N = gintonic::vec3f(lFbxNormal).normalize();
			}
			if (!get_element(lFbxTexCoordArray, polyvertex, vertexid, lFbxTexCoord))
			{
				lFbxTexCoord[0] = lFbxTexCoord[1] = 0.0f;
			}
			if (!get_element(lFbxTangentArray, polyvertex, vertexid, lFbxTangent))
			{
				T = 0.0f;
				mHasTangentsAndBitangents = false;
			}
			else
			{
				T = gintonic::vec3f(lFbxTangent).normalize();
				mHasTangentsAndBitangents = true;
			}
			if (!get_element(lFbxBitangentArray, polyvertex, vertexid, lFbxBitangent))
			{
				B = 0.0f;
			}
			else
			{
				B = gintonic::vec3f(lFbxBitangent).normalize();
			}

			handedness = gintonic::distance(cross(N,T), B) < 0.01f ? 1.0f : -1.0f;

			lSlot0Entry =
			{
				static_cast<GLfloat>(lFbxPosition[0]),
				static_cast<GLfloat>(lFbxPosition[1]),
				static_cast<GLfloat>(lFbxPosition[2]),
				static_cast<GLfloat>(lFbxTexCoord[0])
			};

			lSlot1Entry =
			{
				N.x,
				N.y,
				N.z,
				static_cast<GLfloat>(lFbxTexCoord[1])
			};

			if (mHasTangentsAndBitangents) lSlot2Entry = {T.x, T.y, T.z, handedness};

			bool lFoundDuplicate = false;
			GLuint lIndex;
			for (lIndex = 0; lIndex < lSlot0Array.size(); ++lIndex)
			{
				if (lSlot0Array[lIndex] == lSlot0Entry && lSlot1Array[lIndex] == lSlot1Entry)
				{
					if (mHasTangentsAndBitangents)
					{
						if (lSlot2Array[lIndex] == lSlot2Entry)
						{
							lFoundDuplicate = true;
							break;
						}
					}
					else
					{
						lFoundDuplicate = true;
						break;
					}
				}
			}
			if (!lFoundDuplicate)
			{
				lSlot0Array.push_back(lSlot0Entry);
				lSlot1Array.emplace_back(lSlot1Entry);
				if (mHasTangentsAndBitangents) lSlot2Array.push_back(lSlot2Entry);
			}
			lIndexArray.push_back(lIndex);
			++vertexid;
		}
	}

	std::cerr << "\t\tNumber of vertices: " << lSlot0Array.size() << '\n';
	std::cerr << "\t\tNumber of indices: " << lIndexArray.size()
		<< " (saved " << static_cast<float>(lSlot0Array.size()) 
		/ static_cast<float>(lIndexArray.size()) << ")\n";

	if (mHasTangentsAndBitangents)
	{
		set(lIndexArray, lSlot0Array, lSlot1Array, lSlot2Array);
	}
	else
	{
		set(lIndexArray, lSlot0Array, lSlot1Array);
	}
}

void Mesh::set(
	const std::vector<GLuint>& indices, 
	const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y)
{
	constexpr GLenum lUsageHint = GL_STATIC_DRAW;

	// Enable the Vertex Array Object.
	glBindVertexArray(mVertexArrayObject);

	// Upload the (packed) vertex data in separate buffers to the GPU.
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POS_XYZ_UV_X]);
	gtBufferData(GL_ARRAY_BUFFER, position_XYZ_uv_X, lUsageHint);
	Mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_SLOT_0);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_NOR_XYZ_UV_Y]);
	gtBufferData(GL_ARRAY_BUFFER, normal_XYZ_uv_Y, lUsageHint);
	Mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_SLOT_1);

	// Upload the indices for the arrays to the GPU and remember the count.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES]);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, lUsageHint);

	mHasTangentsAndBitangents = false;
	mNumIndices = static_cast<GLsizei>(indices.size());
	computeLocalBoundingBoxFromPositionInformation(position_XYZ_uv_X);
}

void Mesh::set(
	const std::vector<GLuint>& indices, 
	const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y,
	const std::vector<Mesh::vec4f>& tangent_XYZ_handedness)
{
	constexpr GLenum lUsageHint = GL_STATIC_DRAW;

	// Enable the Vertex Array Object.
	glBindVertexArray(mVertexArrayObject);

	// Upload the (packed) vertex data in separate buffers to the GPU.
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POS_XYZ_UV_X]);
	gtBufferData(GL_ARRAY_BUFFER, position_XYZ_uv_X, lUsageHint);
	Mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_SLOT_0);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_NOR_XYZ_UV_Y]);
	gtBufferData(GL_ARRAY_BUFFER, normal_XYZ_uv_Y, lUsageHint);
	Mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_SLOT_1);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_TAN_XYZ_HAND]);
	gtBufferData(GL_ARRAY_BUFFER, tangent_XYZ_handedness, lUsageHint);
	Mesh::vec4f::enable_attribute(GT_VERTEX_LAYOUT_SLOT_2);

	// Upload the indices for the arrays to the GPU and remember the count.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES]);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, lUsageHint);

	mHasTangentsAndBitangents = true;
	mNumIndices = static_cast<GLsizei>(indices.size());
	computeLocalBoundingBoxFromPositionInformation(position_XYZ_uv_X);
}

void Mesh::set(const boost::filesystem::path& nativeMeshFile)
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void Mesh::draw() const noexcept
{
	glBindVertexArray(mVertexArrayObject);
	glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, nullptr);
}

void Mesh::draw(
	const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
	const std::vector<mat4f, allocator<mat4f>>& VM_matrices,
	const std::vector<mat3f, allocator<mat3f>>& N_matrices)
{
	glBindVertexArray(mVertexArrayObject);

	mMatrixBuffer.bind(0);
	mMatrixBuffer.set(0, PVM_matrices, GL_DYNAMIC_DRAW);

	mMatrixBuffer.bind(1);
	mMatrixBuffer.set(1, VM_matrices, GL_DYNAMIC_DRAW);

	mNormalMatrixBuffer.bind();
	mNormalMatrixBuffer.set(N_matrices, GL_DYNAMIC_DRAW);

	glDrawElementsInstanced(
		GL_TRIANGLES, 
		mNumIndices,
		GL_UNSIGNED_INT,
		nullptr, 
		mMatrixBuffer.size(0)
	);
}

void Mesh::setupInstancedRenderingMatrices() noexcept
{
	glBindVertexArray(mVertexArrayObject);
	mMatrixBuffer.bind(0);
	for (GLuint i = 0; i < 4; ++i)
	{
		glEnableVertexAttribArray(GT_VERTEX_LAYOUT_SLOT_3_4_5_6 + i);

		glVertexAttribPointer(GT_VERTEX_LAYOUT_SLOT_3_4_5_6 + i, 4, 
			GL_FLOAT, GL_FALSE, sizeof(mat4f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		
		glVertexAttribDivisor(GT_VERTEX_LAYOUT_SLOT_3_4_5_6 + i, 1);
	}
	mMatrixBuffer.bind(1);
	for (GLuint i = 0; i < 4; ++i)
	{
		glEnableVertexAttribArray(GT_VERTEX_LAYOUT_SLOT_7_8_9_10 + i);
		
		glVertexAttribPointer(GT_VERTEX_LAYOUT_SLOT_7_8_9_10 + i, 4, 
			GL_FLOAT, GL_FALSE, sizeof(mat4f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		
		glVertexAttribDivisor(GT_VERTEX_LAYOUT_SLOT_7_8_9_10 + i, 1);
	}
	mNormalMatrixBuffer.bind();
	for (GLuint i = 0; i < 3; ++i)
	{
		glEnableVertexAttribArray(GT_VERTEX_LAYOUT_SLOT_11_12_13 + i);
		
		glVertexAttribPointer(GT_VERTEX_LAYOUT_SLOT_11_12_13 + i, 3,
			GL_FLOAT, GL_FALSE, sizeof(mat3f), (const GLvoid*)(sizeof(GLfloat) * i * 3));
		
		glVertexAttribDivisor(GT_VERTEX_LAYOUT_SLOT_11_12_13 + i, 1);
	}
}

void Mesh::computeLocalBoundingBoxFromPositionInformation(const std::vector<Mesh::vec4f>& position_XYZ_uv_X)
{
	// Reset to zero.
	mLocalBoundingBox.min_corner = 0.0f;
	mLocalBoundingBox.max_corner = 0.0f;

	for (const auto v : position_XYZ_uv_X)
	{
		if (v.x < mLocalBoundingBox.min_corner.x) mLocalBoundingBox.min_corner.x = v.x;
		if (v.y < mLocalBoundingBox.min_corner.y) mLocalBoundingBox.min_corner.y = v.y;
		if (v.z < mLocalBoundingBox.min_corner.z) mLocalBoundingBox.min_corner.z = v.z;
		if (v.x > mLocalBoundingBox.max_corner.x) mLocalBoundingBox.max_corner.x = v.x;
		if (v.y > mLocalBoundingBox.max_corner.y) mLocalBoundingBox.max_corner.y = v.y;
		if (v.z > mLocalBoundingBox.max_corner.z) mLocalBoundingBox.max_corner.z = v.z;

		gintonic::vec3f lPosition(v.x, v.y, v.z);

		assert(mLocalBoundingBox.contains(lPosition));
	}
}

} // namespace gintonic