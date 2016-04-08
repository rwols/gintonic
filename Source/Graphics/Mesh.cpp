#include "Mesh.hpp"

#include "../Foundation/tuple.hpp"
#include "../Foundation/exception.hpp"

#include <fbxsdk.h>

#include <map>

#define GT_MESH_BUFFER_POS_XYZ_UV_X 0     // Buffer for the positions and uv.x
#define GT_MESH_BUFFER_NOR_XYZ_UV_Y 1     // Buffer for the normals and uv.y
#define GT_MESH_BUFFER_TAN_XYZ_HAND 2     // Buffer for the tangents and handedness
#define GT_MESH_BUFFER_POSITIONS    3     // Buffer for the position vectors (needed for adjacency)
#define GT_MESH_BUFFER_INDICES      4     // Buffer for the indices
#define GT_MESH_BUFFER_INDICES_ADJ  5     // Buffer for the indices with adjacency information

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

struct Edge
{
	GLuint first;
	GLuint second;

	Edge() = delete;

	Edge(const GLuint positionIndexOne, const GLuint positionIndexTwo)
	{
		if (positionIndexOne < positionIndexTwo)
		{
			first = positionIndexOne;
			second = positionIndexTwo;
		}
		else if (positionIndexOne > positionIndexTwo)
		{
			first = positionIndexTwo;
			second = positionIndexOne;
		}
		else throw std::logic_error("Position indices are the same!");
	}

	bool operator < (const Edge& other) const noexcept
	{
		return std::tie(first, second) < std::tie(other.first, other.second);
	}
};

struct Triangle
{
	GLuint indices[3];

	GLuint getOppositeIndex(const Edge& edge) const
	{
		for (GLuint i = 0; i < 3; ++i)
		{
			const auto lIndex = indices[i];
			if (lIndex != edge.first && lIndex != edge.second)
			{
				return lIndex;
			}
		}
		throw std::logic_error("Triangle has no opposite position index!");
	}

	inline GLuint& operator[](const std::size_t index) noexcept
	{
		return indices[index];
	}

	inline const GLuint& operator[](const std::size_t index) const noexcept
	{
		return indices[index];
	}

	bool operator < (const Triangle& other) const noexcept
	{
		return std::tie(indices[0], indices[1], indices[2]) 
			< std::tie(other[0], other[1], other[2]);
	}
};

std::ostream& operator << (std::ostream& os, const Triangle& triangle)
{
	return os << '[' << triangle[0] 
		<< ", " << triangle[1] 
		<< ", " << triangle[2] << ']';
}

std::ostream& operator << (std::ostream& os, const Edge& edge)
{
	return os << '[' << edge.first << " -> " << edge.second << ']';
}

struct NeighborPair
{
	const Triangle* first = nullptr;
	const Triangle* second = nullptr;

	void addNeighbor(const Triangle* triangle)
	{
		if (!first) first = triangle;
		else if (!second) second = triangle;
		else throw std::logic_error("Both neighbors already set");
	}

	const Triangle* getOther(const Triangle* me) const
	{
		if (first == me) return second;
		else if (second == me) return first;
		else throw std::logic_error("getOther failed");
	}
};

std::ostream& operator << (std::ostream& os, const NeighborPair& neighborPair)
{
	os << '(';
	if (neighborPair.first) os << *(neighborPair.first);
	else os << "nullptr";
	os << ", ";
	if (neighborPair.second) os << *(neighborPair.second);
	else os << "nullptr";
	return os << ')';
}

template <class LayerElement, class VectorType> 
bool getLayerElement(
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

void Mesh::vec2f::enableAttribute(const GLuint index) noexcept
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

bool Mesh::vec3f::operator<(const Mesh::vec3f& other) const noexcept
{
	return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
}

void Mesh::vec3f::enableAttribute(const GLuint index) noexcept
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

bool Mesh::vec4f::operator<(const Mesh::vec4f& other) const noexcept
{
	return std::tie(x, y, z, w) < std::tie(other.x, other.y, other.z, other.w);
}

void Mesh::vec4f::enableAttribute(const GLuint index) noexcept
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec4f), nullptr);
}

Mesh::Mesh()
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
// , mHasTangentsAndBitangents(false)
// , mNumIndices(0)
{
	setupInstancedRenderingMatrices();
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

void Mesh::set(const FbxMesh* pFbxMesh)
{
	setNameWithFbx(pFbxMesh);

	std::cerr << "\tFound mesh: " << this->name << '\n';

	int i, j, polyvertex, polygonsize, polygoncount, vertexid = 0;

	if (pFbxMesh->IsTriangleMesh() == false)
	{
		exception e(pFbxMesh->GetNode()->GetName());
		e.append(": not fully triangulated.");
		throw e;
	}

	bool lHasTangents;

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

	std::map<Edge, NeighborPair> lEdgeToNeighborMap;
	std::map<Mesh::vec3f, GLuint> lPositionToIndexMap;
	std::set<Triangle> lTriangles;
	std::set<Edge> lEdges;

	// Reset the bounding box.
	mLocalBoundingBox.minCorner = std::numeric_limits<float>::max();
	mLocalBoundingBox.maxCorner = std::numeric_limits<float>::min();

	// For each triangle ...
	polygoncount = pFbxMesh->GetPolygonCount();
	for (i = 0; i < polygoncount; ++i)
	{
		Triangle lTriangle;
		polygonsize = pFbxMesh->GetPolygonSize(i);
		assert(polygonsize == 3);
		// For each vertex in the current triangle ...
		for (j = 0; j < polygonsize; ++j)
		{
			polyvertex = pFbxMesh->GetPolygonVertex(i, j);

			lFbxPosition = pFbxMesh->GetControlPointAt(polyvertex);

			if (!getLayerElement(lFbxNormalArray, polyvertex, vertexid, lFbxNormal))
			{
				N = 0.0f;
			}
			else
			{
				N = gintonic::vec3f(lFbxNormal).normalize();
			}
			if (!getLayerElement(lFbxTexCoordArray, polyvertex, vertexid, lFbxTexCoord))
			{
				lFbxTexCoord[0] = lFbxTexCoord[1] = 0.0f;
			}
			if (!getLayerElement(lFbxTangentArray, polyvertex, vertexid, lFbxTangent))
			{
				T = 0.0f;
				lHasTangents = false;
			}
			else
			{
				T = gintonic::vec3f(lFbxTangent).normalize();
				lHasTangents = true;
			}
			if (!getLayerElement(lFbxBitangentArray, polyvertex, vertexid, lFbxBitangent))
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

			if (lHasTangents) lSlot2Entry = {T.x, T.y, T.z, handedness};

			const Mesh::vec3f lPosition(lFbxPosition[0], lFbxPosition[1], lFbxPosition[2]);
			const gintonic::vec3f lPositionAsGTVec(lFbxPosition[0], lFbxPosition[1], lFbxPosition[2]);

			// Add the position to the mesh's local bounding box.
			mLocalBoundingBox.addPoint(lPositionAsGTVec);
			assert(mLocalBoundingBox.contains(lPositionAsGTVec));

			GLuint lIndex;
			auto lPosIter = lPositionToIndexMap.find(lPosition); // O(log n) complexity
			if (lPosIter == lPositionToIndexMap.end())
			{
				lIndex = static_cast<GLuint>(lPositionToIndexMap.size());
				lPositionToIndexMap.insert({lPosition, lIndex});
			}
			else
			{
				lIndex = lPosIter->second;
			}

			lTriangle.indices[j] = lIndex;

			bool lFoundDuplicate = false;
			for (lIndex = 0; lIndex < mPosition_XYZ_uv_X.size(); ++lIndex)
			{
				if (mPosition_XYZ_uv_X[lIndex] == lSlot0Entry && mNormal_XYZ_uv_Y[lIndex] == lSlot1Entry)
				{
					if (lHasTangents)
					{
						if (mTangent_XYZ_hand[lIndex] == lSlot2Entry)
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
				mPosition_XYZ_uv_X.push_back(lSlot0Entry);
				mNormal_XYZ_uv_Y.push_back(lSlot1Entry);
				if (lHasTangents) mTangent_XYZ_hand.push_back(lSlot2Entry);
			}
			mIndices.push_back(lIndex);

			++vertexid;
		}

		std::cerr << "Adding triangle " << lTriangle << '\n';

		const Edge e1(lTriangle[0], lTriangle[1]);
		const Edge e2(lTriangle[1], lTriangle[2]);
		const Edge e3(lTriangle[2], lTriangle[0]);
		auto lInsertionResult = lTriangles.emplace(lTriangle).first;
		lEdgeToNeighborMap[e1].addNeighbor(&(*lInsertionResult));
		lEdgeToNeighborMap[e2].addNeighbor(&(*lInsertionResult));
		lEdgeToNeighborMap[e3].addNeighbor(&(*lInsertionResult));
	}

	for (const auto& lKeyValue : lEdgeToNeighborMap)
	{
		std::cout << lKeyValue.first << " => " << lKeyValue.second << '\n';
	}

	mIndicesAdjacent.clear();

	bool lMeshHasBoundary = false;
	for (const auto& lTriangle : lTriangles)
	{
		if (lMeshHasBoundary) break;
		for (GLuint j = 0; j < 3; ++j)
		{
			const Edge lEdge(lTriangle[j], lTriangle[(j + 1) % 3]);
			assert(lEdgeToNeighborMap.find(lEdge) != lEdgeToNeighborMap.end());
			const auto lNeighbor = lEdgeToNeighborMap[lEdge];
			const auto* lOtherTriangle = lNeighbor.getOther(&lTriangle);
			if (!lOtherTriangle)
			{
				lMeshHasBoundary = true;
				break;
			}
			mIndicesAdjacent.push_back(lTriangle[j]);
			mIndicesAdjacent.push_back(lOtherTriangle->getOppositeIndex(lEdge));
		}
	}

	mPositions.clear();

	if (lMeshHasBoundary)
	{
		mIndicesAdjacent.clear();
		std::cerr << "\tWARNING: Mesh has holes and/or boundaries.\n"
			<< "\tSilhouette detection algorithm will not work.\n";
	}
	else
	{
		for (const auto& lKeyValue : lPositionToIndexMap)
		{
			mPositions.push_back(lKeyValue.first);
		}
	}

	// Finally, make the positions a proper array


	std::cerr << "\tNumber of vertices: " << mPosition_XYZ_uv_X.size() << '\n';
	std::cerr << "\tNumber of indices: " << mIndices.size()
		<< " (saved " << static_cast<float>(mPosition_XYZ_uv_X.size()) 
		/ static_cast<float>(mIndices.size()) << ")\n";
	std::cerr << "\tNumber of points: " << mPositions.size() << '\n';
	std::cerr << "\tNumber of adjacency indices: " << mIndicesAdjacent.size()
		<< " (saved " << static_cast<float>(mPositions.size()) 
		/ static_cast<float>(mIndicesAdjacent.size()) << ")\n";

	uploadData();
}

void Mesh::set(
	const std::vector<GLuint>& indices, 
	const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y)
{
	mIndices = indices;
	mPosition_XYZ_uv_X = position_XYZ_uv_X;
	mNormal_XYZ_uv_Y = normal_XYZ_uv_Y;
	mTangent_XYZ_hand.clear();
	computeLocalBoundingBoxFromPositionInformation(mPosition_XYZ_uv_X);
	uploadData();
}

void Mesh::set(
	const std::vector<GLuint>& indices, 
	const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y,
	const std::vector<Mesh::vec4f>& tangent_XYZ_handedness)
{
	mIndices = indices;
	mPosition_XYZ_uv_X = position_XYZ_uv_X;
	mNormal_XYZ_uv_Y = normal_XYZ_uv_Y;
	mTangent_XYZ_hand = tangent_XYZ_handedness;
	computeLocalBoundingBoxFromPositionInformation(mPosition_XYZ_uv_X);
	uploadData();
}

void Mesh::set(const boost::filesystem::path& nativeMeshFile)
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
}

void Mesh::draw() const noexcept
{
	glBindVertexArray(mVertexArrayObject);
	glDrawElements(GL_TRIANGLES, numIndices(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::drawAdjacent() const noexcept
{
	glBindVertexArray(mVertexArrayObjectAdjacencies);
	glDrawElements(GL_TRIANGLES_ADJACENCY, numIndicesAdjacent(), GL_UNSIGNED_INT, nullptr);
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
		numIndices(),
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
	// Reset the bounding box.
	mLocalBoundingBox.minCorner = std::numeric_limits<float>::max();
	mLocalBoundingBox.maxCorner = std::numeric_limits<float>::min();

	for (const auto v : position_XYZ_uv_X)
	{
		const gintonic::vec3f lPosition(v.x, v.y, v.z);
		mLocalBoundingBox.addPoint(lPosition);
		assert(mLocalBoundingBox.contains(lPosition));
	}
}

void Mesh::computeAdjacencyFromPositionInformation()
{
	mIndicesAdjacent.clear();
	mPositions.clear();
	std::set<Mesh::vec3f> lUniquePositions;
	for (const auto& lVec4 : mPosition_XYZ_uv_X)
	{
		const Mesh::vec3f lVec3(lVec4.x, lVec4.x, lVec4.y);
		lUniquePositions.insert(lVec3);
	}
	std::copy(lUniquePositions.begin(), lUniquePositions.end(), std::back_inserter(mPositions));
	for (const auto& lPosition : mPositions)
	{
		std::cout << lPosition << '\n';
	}
}

void Mesh::uploadData()
{
	constexpr GLenum lUsageHint = GL_STATIC_DRAW;
	glBindVertexArray(mVertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES]);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices, lUsageHint);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POS_XYZ_UV_X]);
	gtBufferData(GL_ARRAY_BUFFER, mPosition_XYZ_uv_X, lUsageHint);
	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_0);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_NOR_XYZ_UV_Y]);
	gtBufferData(GL_ARRAY_BUFFER, mNormal_XYZ_uv_Y, lUsageHint);
	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_1);
	if (hasTangentsAndBitangents())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_TAN_XYZ_HAND]);
		gtBufferData(GL_ARRAY_BUFFER, mTangent_XYZ_hand, lUsageHint);
		Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_2);
	}
	glBindVertexArray(mVertexArrayObjectAdjacencies);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES_ADJ]);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicesAdjacent, lUsageHint);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POSITIONS]);
	gtBufferData(GL_ARRAY_BUFFER, mPositions, lUsageHint);
	Mesh::vec3f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_0);
}

// void Mesh::setFromSerialized(
// 	const std::vector<GLuint>& indices,
// 	const std::vector<Mesh::vec4f>& position_XYZ_uv_X,
// 	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y)
// {
// 	// Don't set the count, the bounding box and wether we have tangents.
// 	constexpr GLenum lUsageHint = GL_STATIC_DRAW;

// 	// Enable the Vertex Array Object.
// 	glBindVertexArray(mVertexArrayObject);

// 	// Upload the (packed) vertex data in separate buffers to the GPU.
// 	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POS_XYZ_UV_X]);
// 	gtBufferData(GL_ARRAY_BUFFER, position_XYZ_uv_X, lUsageHint);
// 	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_0);
// 	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_NOR_XYZ_UV_Y]);
// 	gtBufferData(GL_ARRAY_BUFFER, normal_XYZ_uv_Y, lUsageHint);
// 	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_1);

// 	// Upload the indices for the arrays to the GPU and remember the count.
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES]);
// 	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, lUsageHint);
// }

// void Mesh::setFromSerialized(
// 	const std::vector<GLuint>& indices,
// 	const std::vector<Mesh::vec4f>& position_XYZ_uv_X,
// 	const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y,
// 	const std::vector<Mesh::vec4f>& tangent_XYZ_handedness)
// {
// 	// Don't set the count, the bounding box and wether we have tangents.
// 	constexpr GLenum lUsageHint = GL_STATIC_DRAW;

// 	// Enable the Vertex Array Object.
// 	glBindVertexArray(mVertexArrayObject);

// 	// Upload the (packed) vertex data in separate buffers to the GPU.
// 	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POS_XYZ_UV_X]);
// 	gtBufferData(GL_ARRAY_BUFFER, position_XYZ_uv_X, lUsageHint);
// 	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_0);
// 	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_NOR_XYZ_UV_Y]);
// 	gtBufferData(GL_ARRAY_BUFFER, normal_XYZ_uv_Y, lUsageHint);
// 	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_1);
// 	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_TAN_XYZ_HAND]);
// 	gtBufferData(GL_ARRAY_BUFFER, tangent_XYZ_handedness, lUsageHint);
// 	Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_2);

// 	// Upload the indices for the arrays to the GPU and remember the count.
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES]);
// 	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, indices, lUsageHint);
// }

} // namespace gintonic