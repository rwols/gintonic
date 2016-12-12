#include "Graphics/Mesh.hpp"

#include "Foundation/tuple.hpp"
#include "Foundation/exception.hpp"

#include "Graphics/Skeleton.hpp"

#include <fbxsdk.h>

#include <set>

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

// std::ostream& operator << (std::ostream& os, const Triangle& triangle)
// {
// 	return os << '[' << triangle[0] 
// 		<< ", " << triangle[1] 
// 		<< ", " << triangle[2] << ']';
// }

// std::ostream& operator << (std::ostream& os, const Edge& edge)
// {
// 	return os << '[' << edge.first << " -> " << edge.second << ']';
// }

struct NeighborPair
{
	const Triangle* first = nullptr;
	const Triangle* second = nullptr;

	bool addNeighbor(const Triangle* triangle)
	{
		if (!first)
		{
			first = triangle;
			return true;
		}
		else if (!second)
		{
			second = triangle;
			return true;
		}
		else
		{
			return false;
		}
	}

	const Triangle* getOther(const Triangle* me) const
	{
		if (first == me) return second;
		else if (second == me) return first;
		else throw std::logic_error("getOther failed");
	}
};

// std::ostream& operator << (std::ostream& os, const NeighborPair& neighborPair)
// {
// 	os << '(';
// 	if (neighborPair.first) os << *(neighborPair.first);
// 	else os << "nullptr";
// 	os << ", ";
// 	if (neighborPair.second) os << *(neighborPair.second);
// 	else os << "nullptr";
// 	return os << ')';
// }

template <class LayerElement, class OutputIter>
void copyLayerElement(
	const LayerElement* layer,
	OutputIter iter)
{
	if (!layer) return;
	const auto& lElements = layer->GetDirectArray();
	switch (layer->GetReferenceMode())
	{
		case FbxGeometryElement::eDirect:
		{
			for (int i = 0; i < lElements.GetCount(); ++i)
			{
				*iter = lElements[i];
				++iter;
			}
			break;
		}
		case FbxGeometryElement::eIndex: // fall through
		case FbxGeometryElement::eIndexToDirect:
		{
			const auto& lIndices = layer->GetIndexArray();
			for (int i = 0; i < lIndices.GetCount(); ++i)
			{
				*iter = lElements[lIndices[i]];
				++iter;
			}
			break;
		}
	}
}

template <class LayerElement, class VectorType> 
bool getLayerElement(
	const LayerElement* layerElement, 
	const int lPolygonID,
	const int lVertexID,
	VectorType& r)
{
	if (!layerElement) return false;
	switch(layerElement->GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (layerElement->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					r = layerElement->GetDirectArray().GetAt(lPolygonID);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					const auto lIndex2Element = layerElement->GetIndexArray().GetAt(lPolygonID);
					r = layerElement->GetDirectArray().GetAt(lIndex2Element);
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
			switch(layerElement->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
				{
					r = layerElement->GetDirectArray().GetAt(lVertexID);
					break;
				}
				case FbxGeometryElement::eIndexToDirect:
				{
					const auto lIndex2Element = layerElement->GetIndexArray().GetAt(lVertexID);
					r = layerElement->GetDirectArray().GetAt(lIndex2Element);
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

Mesh::vec4i::vec4i(const FbxVector4& v)
: x(static_cast<GLint>(v[0]))
, y(static_cast<GLint>(v[1]))
, z(static_cast<GLint>(v[2]))
, w(static_cast<GLint>(v[3]))
{
	/* Empty on purpose. */
}

Mesh::vec4i::vec4i(const GLint x, const GLint y, const GLint z, const GLint w)
: x(x)
, y(y)
, z(z)
, w(w)
{
	/* Empty on purpose. */
}

bool Mesh::vec4i::operator==(const Mesh::vec4i& other) const noexcept
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Mesh::vec4i::operator!=(const Mesh::vec4i& other) const noexcept
{
	return !operator==(other);
}

bool Mesh::vec4i::operator<(const Mesh::vec4i& other) const noexcept
{
	return std::tie(x, y, z, w) < std::tie(other.x, other.y, other.z, other.w);
}

void Mesh::vec4i::enableAttribute(const GLuint index) noexcept
{
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, 4, GL_INT, sizeof(Mesh::vec4i), nullptr);
}

Mesh::Mesh()
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
}

Mesh::Mesh(FbxMesh* pFbxMesh)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(pFbxMesh);
}

Mesh::Mesh(
	FbxMesh* pFbxMesh,
	Skeleton& skeleton)
: mMatrixBuffer(GL_DYNAMIC_DRAW)
, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
	setupInstancedRenderingMatrices();
	set(pFbxMesh, skeleton);
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

void Mesh::buildFromFbxMesh(FbxMesh* pFbxMesh, std::multimap<int, GLuint>* fbxIndicesToOwnMap)
{
	setNameWithFbx(pFbxMesh);

	std::cerr << "\tFound mesh: " << this->name << '\n';

	int i, j, lFbxIndex, lPolygonSize, lTriangleCount, lVertexID = 0;

	if (pFbxMesh->IsTriangleMesh() == false)
	{
		exception e(pFbxMesh->GetNode()->GetName());
		e.append(": not fully triangulated.");
		throw e;
	}

	bool lHasTangents;
	bool lMoreThan2Neighbors = false;

	Mesh::vec4f lSlot0Entry;
	Mesh::vec4f lSlot1Entry;
	Mesh::vec4f lSlot2Entry{0.0f, 0.0f, 0.0f, 1.0f};

	FbxVector4 lFbxPosition;
	FbxVector2 lFbxTexCoord;
	FbxVector4 lFbxNormal;
	FbxVector4 lFbxTangent;
	FbxVector4 lFbxBitangent;

	std::vector<FbxVector2> lFbxTexCoords;
	std::vector<FbxVector4> lFbxNormals;
	std::vector<FbxVector4> lFbxTangents;
	std::vector<FbxVector4> lFbxBitangents;

	gintonic::vec3f N;
	gintonic::vec3f T;
	gintonic::vec3f B;

	GLfloat lHandedness;

	using VertexEntry = std::tuple<Mesh::vec4f, Mesh::vec4f, Mesh::vec4f>;

	std::map<VertexEntry, GLuint> lVertexToIndexMap;

	// Reset the bounding box.
	mLocalBoundingBox.minCorner = std::numeric_limits<float>::max();
	mLocalBoundingBox.maxCorner = std::numeric_limits<float>::min();

	FbxGeometryElementUV*       lFbxTexCoordArray  = nullptr;
	FbxGeometryElementNormal*   lFbxNormalArray    = nullptr;
	FbxGeometryElementTangent*  lFbxTangentArray   = nullptr;
	FbxGeometryElementBinormal* lFbxBitangentArray = nullptr;

	FbxStringList lUVSetNames;
	pFbxMesh->GetUVSetNames(lUVSetNames);

	if (lUVSetNames.GetCount()) 
	{
		lFbxTexCoordArray = pFbxMesh->GetElementUV(lUVSetNames.GetStringAt(0));
	}
	if (!pFbxMesh->GenerateNormals())
	{
		throw std::runtime_error("could not generate normals.");
	}
	if (pFbxMesh->GetElementNormalCount()) 
	{
		lFbxNormalArray = pFbxMesh->GetElementNormal(0);
	}
	else
	{
		throw std::runtime_error("mesh has no normals");
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

	mIndices.clear();
	mPosition_XYZ_uv_X.clear();
	mNormal_XYZ_uv_Y.clear();
	mTangent_XYZ_hand.clear();

	assert(3 * pFbxMesh->GetPolygonCount() == pFbxMesh->GetPolygonVertexCount());
	std::vector<GLuint> lFbxIndicesLHS(pFbxMesh->GetPolygonVertices(), pFbxMesh->GetPolygonVertices() + pFbxMesh->GetPolygonVertexCount());
	std::vector<GLuint> lFbxIndicesRHS;
	lFbxIndicesRHS.reserve(3 * pFbxMesh->GetPolygonCount());

	// For each triangle ...
	lTriangleCount = pFbxMesh->GetPolygonCount();
	for (i = 0; i < lTriangleCount; ++i)
	{
		Triangle lTriangle;
		lPolygonSize = pFbxMesh->GetPolygonSize(i);
		assert(lPolygonSize == 3);
		// For each vertex in the current triangle ...
		for (j = 0; j < lPolygonSize; ++j)
		{
			lFbxIndex = pFbxMesh->GetPolygonVertex(i, j);
			lFbxIndicesRHS.push_back(lFbxIndex);

			lFbxPosition = pFbxMesh->GetControlPointAt(lFbxIndex);

			if (!getLayerElement(lFbxNormalArray, lFbxIndex, lVertexID, lFbxNormal))
			{
				N = 0.0f;
			}
			else
			{
				N = gintonic::vec3f(lFbxNormal).normalize();
			}
			if (!getLayerElement(lFbxTexCoordArray, lFbxIndex, lVertexID, lFbxTexCoord))
			{
				lFbxTexCoord[0] = lFbxTexCoord[1] = 0.0f;
			}
			if (!getLayerElement(lFbxTangentArray, lFbxIndex, lVertexID, lFbxTangent))
			{
				T = 0.0f;
				lHasTangents = false;
			}
			else
			{
				T = gintonic::vec3f(lFbxTangent).normalize();
				lHasTangents = true;
			}
			if (!getLayerElement(lFbxBitangentArray, lFbxIndex, lVertexID, lFbxBitangent))
			{
				B = 0.0f;
			}
			else
			{
				B = gintonic::vec3f(lFbxBitangent).normalize();
			}

			lHandedness = gintonic::distance(cross(N,T), B) < 0.01f ? 1.0f : -1.0f;

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

			if (lHasTangents) lSlot2Entry = {T.x, T.y, T.z, lHandedness};

			Mesh::vec3f lPosition(static_cast<GLfloat>(lFbxPosition[0]), static_cast<GLfloat>(lFbxPosition[1]), static_cast<GLfloat>(lFbxPosition[2]));

			const gintonic::vec3f lPositionAsGTVec(static_cast<GLfloat>(lFbxPosition[0]), static_cast<GLfloat>(lFbxPosition[1]), static_cast<GLfloat>(lFbxPosition[2]));

			// Add the position to the mesh's local bounding box.
			mLocalBoundingBox.addPoint(lPositionAsGTVec);
			assert(mLocalBoundingBox.contains(lPositionAsGTVec));

			GLuint lIndex;
			auto lPosIter = lPositionToIndexMap.find(lPosition); // O(log n) complexity
			if (lPosIter == lPositionToIndexMap.end())
			{
				lIndex = static_cast<GLuint>(lPositionToIndexMap.size());
				lPositionToIndexMap.insert({lPosition, lIndex});
				mPositions.push_back(lPosition);
			}
			else
			{
				lIndex = lPosIter->second;
			}

			lTriangle.indices[j] = lIndex;

			const auto lFindResult = lVertexToIndexMap.find(VertexEntry{lSlot0Entry, lSlot1Entry, lSlot2Entry});
			if (lFindResult == lVertexToIndexMap.end())
			{
				lIndex = static_cast<GLuint>(lVertexToIndexMap.size());
				lVertexToIndexMap.emplace(VertexEntry{lSlot0Entry, lSlot1Entry, lSlot2Entry}, lIndex);
				mPosition_XYZ_uv_X.push_back(lSlot0Entry);
				mNormal_XYZ_uv_Y.push_back(lSlot1Entry);
				if (lHasTangents) mTangent_XYZ_hand.push_back(lSlot2Entry);
			}
			else
			{
				lIndex = lFindResult->second;
			}
			mIndices.push_back(lIndex);

			if (fbxIndicesToOwnMap) fbxIndicesToOwnMap->emplace(lFbxIndex, lIndex);

			++lVertexID;
		}

		const Edge e1(lTriangle[0], lTriangle[1]);
		const Edge e2(lTriangle[1], lTriangle[2]);
		const Edge e3(lTriangle[2], lTriangle[0]);
		auto lInsertionResult = lTriangles.emplace(lTriangle).first;
		if (!lEdgeToNeighborMap[e1].addNeighbor(&(*lInsertionResult))) lMoreThan2Neighbors = true;
		if (!lEdgeToNeighborMap[e2].addNeighbor(&(*lInsertionResult))) lMoreThan2Neighbors = true;
		if (!lEdgeToNeighborMap[e3].addNeighbor(&(*lInsertionResult))) lMoreThan2Neighbors = true;
	}

	assert(pFbxMesh->GetPolygonVertexCount() == static_cast<int>(mIndices.size()));
	assert(lFbxIndicesLHS == lFbxIndicesRHS);

	mIndicesAdjacent.clear();

	bool lMeshHasBoundary = false;
	for (const auto& lTriangle : lTriangles)
	{
		if (lMeshHasBoundary || lMoreThan2Neighbors) break;
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

	if (lMoreThan2Neighbors)
	{
		mPositions.clear();
		mIndicesAdjacent.clear();
		std::cerr << "\tWARNING: Mesh has an edge with more than two neighbors.\n"
			<< "\tSilhouette detection algorithm will not work.\n";
	}
	else if (lMeshHasBoundary)
	{
		mPositions.clear();
		mIndicesAdjacent.clear();
		std::cerr << "\tWARNING: Mesh has holes and/or boundaries.\n"
			<< "\tSilhouette detection algorithm will not work.\n";
	}
	else
	{
		std::cerr << "\tNumber of points: " << mPositions.size() << '\n';
		std::cerr << "\tNumber of adjacency indices: " << mIndicesAdjacent.size()
			<< " (saved " << static_cast<float>(mPositions.size()) 
			/ static_cast<float>(mIndicesAdjacent.size()) << ")\n";
	}
	std::cerr << "\tNumber of vertices: " << mPosition_XYZ_uv_X.size() << '\n';
	std::cerr << "\tNumber of indices: " << mIndices.size()
		<< " (saved " << static_cast<float>(mPosition_XYZ_uv_X.size()) 
		/ static_cast<float>(mIndices.size()) << ")\n";

	pFbxMesh->ComputeBBox();
	mLocalBoundingBox.minCorner = pFbxMesh->BBoxMin.Get();
	mLocalBoundingBox.maxCorner = pFbxMesh->BBoxMax.Get();

	std::cerr << "\tLocal bounding box min: " << mLocalBoundingBox.minCorner << '\n';
	std::cerr << "\tLocal bounding box max: " << mLocalBoundingBox.maxCorner << '\n';
}

void Mesh::set(FbxMesh* pFbxMesh)
{
	buildFromFbxMesh(pFbxMesh, nullptr);
	uploadData();
}

void Mesh::set(FbxMesh* pFbxMesh, Skeleton& skeleton)
{
	std::multimap<int, GLuint> lFbxIndicesToOwnMap;
	buildFromFbxMesh(pFbxMesh, &lFbxIndicesToOwnMap);
	buildSkinningInformation(pFbxMesh, skeleton, lFbxIndicesToOwnMap);
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
	computeAdjacencyFromPositionInformation();
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
	computeAdjacencyFromPositionInformation();
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
	std::map<Edge, NeighborPair> lEdgeToNeighborMap;
	std::map<Mesh::vec3f, GLuint> lPositionToIndexMap;
	std::set<Triangle> lTriangles;

	assert(0.0f == -0.0f);
	assert(mIndices.size() % 3 == 0);

	for (std::size_t i = 0; i < mIndices.size(); i += 3)
	{
		Triangle lTriangle;

		for (GLuint j = 0; j < 3; ++j)
		{
			const Mesh::vec4f lPosition4(mPosition_XYZ_uv_X[mIndices[i + j]]);
			const Mesh::vec3f lPosition(lPosition4.x, lPosition4.y, lPosition4.z);

			GLuint lIndex;

			const auto lPosIter = lPositionToIndexMap.find(lPosition);
			if (lPosIter == lPositionToIndexMap.end())
			{
				lIndex = static_cast<GLuint>(lPositionToIndexMap.size());
				lPositionToIndexMap.insert({lPosition, lIndex});
				mPositions.push_back(lPosition);
			}
			else
			{
				lIndex = lPosIter->second;
			}

			lTriangle[j] = lIndex;
		}

		const Edge e1(lTriangle[0], lTriangle[1]);
		const Edge e2(lTriangle[1], lTriangle[2]);
		const Edge e3(lTriangle[2], lTriangle[0]);
		auto lInsertionResult = lTriangles.emplace(lTriangle).first;
		lEdgeToNeighborMap[e1].addNeighbor(&(*lInsertionResult));
		lEdgeToNeighborMap[e2].addNeighbor(&(*lInsertionResult));
		lEdgeToNeighborMap[e3].addNeighbor(&(*lInsertionResult));
	}

	mIndicesAdjacent.clear();

	const Triangle* lDegenerateTriangle = nullptr;
	for (const auto& lTriangle : lTriangles)
	{
		if (lDegenerateTriangle)
		{
			break;
		}
		for (GLuint j = 0; j < 3; ++j)
		{
			const Edge lEdge(lTriangle[j], lTriangle[(j + 1) % 3]);
			const auto lNeighborFindResult = lEdgeToNeighborMap.find(lEdge);
			if (lNeighborFindResult == lEdgeToNeighborMap.end())
			{
				throw std::logic_error("Adjacency algorithm failure");
			}
			const auto lNeighbor = lNeighborFindResult->second;
			const auto* lOtherTriangle = lNeighbor.getOther(&lTriangle);
			if (!lOtherTriangle)
			{
				lDegenerateTriangle = &lTriangle;
				break;
			}
			mIndicesAdjacent.push_back(lTriangle[j]);
			mIndicesAdjacent.push_back(lOtherTriangle->getOppositeIndex(lEdge));
		}
	}

	if (lDegenerateTriangle)
	{
		mPositions.clear();
		mIndicesAdjacent.clear();
	}
}

void Mesh::buildSkinningInformation(const FbxMesh* pFbxMesh, Skeleton& mySkeleton, const std::multimap<int, GLuint>& fbxIndicesToOwnMap)
{
	mJointIndices.clear();
	mJointWeights.clear();

	if (pFbxMesh->GetDeformerCount() <= 0) return;
	if (pFbxMesh->GetDeformerCount() >  1)
	{
		std::cerr << "\tWARNING: Mesh \"" << name << "\" has more than one deformer.\n";
	}
	switch (pFbxMesh->GetDeformer(0)->GetDeformerType())
	{
		case FbxDeformer::eUnknown:
			std::cerr << "\tUnknown deformer.\n";
			break;
		case FbxDeformer::eSkin:
			std::cerr << "\tSkin deformer.\n";
			break;
		case FbxDeformer::eBlendShape:
			std::cerr << "\tBlend shape deformer.\n";
			break;
		case FbxDeformer::eVertexCache:
			std::cerr << "\tVertex cache deformer.\n";
			break;
		default:
			std::cerr << "\tUnknown deformer.\n";
			break;
	}
	const auto pFbxSkin = FbxCast<FbxSkin>(pFbxMesh->GetDeformer(0));
	if (!pFbxSkin)
	{
		std::cerr << "\tWARNING: Mesh \"" << name << "\" has a deformer, but it is not an FbxSkin.\n";
		return;
	}

	std::map<std::string, std::vector<std::pair<GLuint, GLfloat>>> lJointNameToInfluenceMap;

	const FbxAMatrix lGeometryTransform(
		pFbxMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot),
		pFbxMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot),
		pFbxMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot));

	for (int c = 0; c < pFbxSkin->GetClusterCount(); ++c)
	{
		const auto lCluster   = pFbxSkin->GetCluster(c);
		const auto TheirJoint = lCluster->GetLink();
		const auto lMyJoint   = mySkeleton.findJointByName(TheirJoint->GetName());

		if (lMyJoint == mySkeleton.end())
		{
			std::cerr << "\tWARNING: Joint \"" << TheirJoint->GetName() 
				<< "\" is not part of the skeleton \"" << mySkeleton.name << "\".\n";
			continue;
		}

		FbxAMatrix lTransform;
		FbxAMatrix lTransformLink;

		lCluster->GetTransformMatrix(lTransform);
		lCluster->GetTransformLinkMatrix(lTransformLink);

		lMyJoint->inverseBindPose = mat4f(lTransformLink.Inverse() * lTransform * lGeometryTransform);

		auto& lJointInfluence = lJointNameToInfluenceMap[lMyJoint->name];
	
		for (int i = 0; i < lCluster->GetControlPointIndicesCount(); ++i)
		{
			const auto lMyIndexRange = fbxIndicesToOwnMap.equal_range(lCluster->GetControlPointIndices()[i]);
			assert(lMyIndexRange.first != lMyIndexRange.second);
			for (auto lRangeIter = lMyIndexRange.first; lRangeIter != lMyIndexRange.second; ++lRangeIter)
			{
				lJointInfluence.emplace_back
				(
					lRangeIter->second, 
					static_cast<GLfloat>(lCluster->GetControlPointWeights()[i])
				);
			}
		}

		std::cerr << "\tFound joint influence for joint \""  
			<< lMyJoint->name << "\" of skeleton \"" << mySkeleton.name << "\".\n";
	}

	std::cerr << mySkeleton << '\n';

	mJointIndices.resize(mPosition_XYZ_uv_X.size());
	mJointWeights.resize(mPosition_XYZ_uv_X.size());

	std::fill(mJointIndices.begin(), mJointIndices.end(), Mesh::vec4i(static_cast<int>(GT_JOINT_NONE), static_cast<int>(GT_JOINT_NONE), static_cast<int>(GT_JOINT_NONE), static_cast<int>(GT_JOINT_NONE)));
	std::fill(mJointWeights.begin(), mJointWeights.end(), Mesh::vec4f(0.0f, 0.0f, 0.0f, 0.0f));

	for (uint8_t lJointIndex = 0; lJointIndex < static_cast<uint8_t>(mySkeleton.joints.size()); ++lJointIndex)
	{
		const auto& lJointInfluence = lJointNameToInfluenceMap[mySkeleton[lJointIndex].name];
		if (lJointInfluence.empty())
		{
			std::cerr << "\tJoint \"" << mySkeleton[lJointIndex].name << "\" influences no vertices at all.\n";
			continue;
		}
		for (const auto& lPair : lJointInfluence)
		{
			const auto lVertexIndex  = lPair.first;
			const auto lVertexWeight = lPair.second;

			if (mJointIndices[lVertexIndex].x == static_cast<int>(lJointIndex) ||
				mJointIndices[lVertexIndex].y == static_cast<int>(lJointIndex) ||
				mJointIndices[lVertexIndex].z == static_cast<int>(lJointIndex) ||
				mJointIndices[lVertexIndex].w == static_cast<int>(lJointIndex))
			{
				continue;
			}
			if (mJointIndices[lVertexIndex].x == GT_JOINT_NONE)
			{
				mJointIndices[lVertexIndex].x = static_cast<int>(lJointIndex);
				mJointWeights[lVertexIndex].x = lVertexWeight;
			}
			else if (mJointIndices[lVertexIndex].y == GT_JOINT_NONE)
			{
				mJointIndices[lVertexIndex].y = static_cast<int>(lJointIndex);
				mJointWeights[lVertexIndex].y = lVertexWeight;
			}
			else if (mJointIndices[lVertexIndex].z == GT_JOINT_NONE)
			{
				mJointIndices[lVertexIndex].z = static_cast<int>(lJointIndex);
				mJointWeights[lVertexIndex].z = lVertexWeight;
			}
			else if (mJointIndices[lVertexIndex].w == GT_JOINT_NONE)
			{
				mJointIndices[lVertexIndex].w = static_cast<int>(lJointIndex);
				mJointWeights[lVertexIndex].w = lVertexWeight;
			}
			else
			{
				std::cerr << "\tWARNING: Vertex " << lVertexIndex << " is influenced by more than 4 joints:\n";
				std::cerr << "\t\t" << mySkeleton[mJointIndices[lVertexIndex].x].name << '\n';
				std::cerr << "\t\t" << mySkeleton[mJointIndices[lVertexIndex].y].name << '\n';
				std::cerr << "\t\t" << mySkeleton[mJointIndices[lVertexIndex].z].name << '\n';
				std::cerr << "\t\t" << mySkeleton[mJointIndices[lVertexIndex].w].name << '\n';
				std::cerr << "\t\t" << mySkeleton[lJointIndex].name << '\n';
			}
		}
	}
}

// void Mesh::buildBonesArray(const FbxMesh* pFbxMesh, const std::map<int, GLuint>& fbxIndicesToOwnMap)
// {
// 	std::map<std::string, Bone::IndexType> lBoneNameToIndexMap;
// 	std::map<Bone::IndexType, const FbxNode*> lIndexToBoneMap;
// 	std::map<const FbxNode*, std::vector<std::pair<GLuint, GLfloat>>> lBoneToInfluenceMap;
// 	std::map<const FbxNode*, const FbxCluster*> lBoneToClusterMap;
// 	const FbxNode* lRootBone = nullptr;

// 	skeleton.clear();
// 	mJointIndices.clear();
// 	mJointWeights.clear();

// 	if (pFbxMesh->GetDeformerCount() <= 0)
// 	{
// 		return;
// 	}

// 	mJointIndices.resize(mPosition_XYZ_uv_X.size());
// 	mJointWeights.resize(mPosition_XYZ_uv_X.size());

// 	std::fill(mJointIndices.begin(), mJointIndices.end(), Mesh::vec4i(0, 0, 0, 0));
// 	std::fill(mJointWeights.begin(), mJointWeights.end(), Mesh::vec4f(0.0f, 0.0f, 0.0f, 0.0f));

// 	buildBonesRecursive(pFbxMesh->GetNode(), static_cast<Bone::IndexType>(-1), lBoneNameToIndexMap);
// 	if (skeleton.empty())
// 	{
// 		// Last resort to find bones.
// 		// This can have the unfortunate effect that multiple meshes
// 		// could have the same skeleton.
// 		buildBonesRecursive(pFbxMesh->GetScene()->GetRootNode(), static_cast<Bone::IndexType>(-1), lBoneNameToIndexMap);
// 	}
	
// 	std::cerr << "\tMesh has " << skeleton.size() << " bones.\n";

// 	for (Bone::IndexType lBone = 0; lBone < static_cast<Bone::IndexType>(skeleton.size()); ++lBone)
// 	{
// 		std::cerr << "\t\tBone " << lBone << ": " << skeleton[lBone].name << ", parent " << skeleton[lBone].parent << '\n';
// 	}

// 	std::cerr << "\tMesh has " << pFbxMesh->GetDeformerCount() << " deformer";
// 	if (pFbxMesh->GetDeformerCount() > 1) std::cerr << "s.\n";
// 	else std::cerr << ".\n";

// 	for (int d = 0; d < pFbxMesh->GetDeformerCount(); ++d)
// 	{
// 		const auto pFbxSkin = FbxCast<FbxSkin>(pFbxMesh->GetDeformer(d));
// 		if (!pFbxSkin)
// 		{
// 			std::cerr << "\tDeformer " << d << " is not an FbxSkin. Skipping...\n";
// 			continue;
// 		}
// 		std::cerr << "\tDeformer " << d << " has " << pFbxSkin->GetClusterCount() << " clusters.\n";
// 		for (int c = 0; c < pFbxSkin->GetClusterCount(); ++c)
// 		{
// 			const auto lCluster    = pFbxSkin->GetCluster(c);
// 			const auto lLink       = lCluster->GetLink();
// 			const auto lFindResult = lBoneNameToIndexMap.find(lLink->GetName());
// 			if (lFindResult == lBoneNameToIndexMap.end())
// 			{
// 				exception lException(this->name);
// 				lException.append(": ");
// 				lException.append("could not find bone with name ");
// 				lException.append(lLink->GetName());
// 				throw lException;
// 			}

// 			FbxAMatrix lTransform;
// 			FbxAMatrix lTransformLink;
// 			FbxAMatrix lGlobalBindPoseInverse;

// 			lCluster->GetTransformMatrix(lTransform);
// 			lCluster->GetTransformLinkMatrix(lTransformLink);

// 			lGlobalBindPoseInverse = lTransformLink.Inverse() * lTransform;

// 			skeleton[lFindResult->second].transform = SQT(lGlobalBindPoseInverse);

// 			std::vector<std::pair<GLuint, GLfloat>> lJointInfluence;
// 			lJointInfluence.reserve(lCluster->GetControlPointIndicesCount());

// 			for (int i = 0; i < lCluster->GetControlPointIndicesCount(); ++i)
// 			{
// 				const auto lFindResult = fbxIndicesToOwnMap.find(lCluster->GetControlPointIndices()[i]);
// 				assert(lFindResult != fbxIndicesToOwnMap.end());

// 				lJointInfluence.emplace_back
// 				(
// 					lFindResult->second, 
// 					static_cast<GLfloat>(lCluster->GetControlPointWeights()[i])
// 				);
// 			}

// 			lJointToInfluenceMap.emplace(lLink, std::move(lJointInfluence));

// 		}

// 		for (int i = 0; i < skeleton.joints.size(); ++i)
// 		{

// 		}
// 	}

// 	// const int lBoneCount = pFbxSkin->GetClusterCount();
// 	// bones.reserve(lBoneCount);

// 	// for (int b = 0; b < lBoneCount; ++b)
// 	// {
// 	// 	const auto lCluster = pFbxSkin->GetCluster(b);
// 	// 	const auto lLink    = lCluster->GetLink();

// 	// 	std::vector<std::pair<GLuint, GLfloat>> lBoneInfluence;
// 	// 	lBoneInfluence.reserve(lCluster->GetControlPointIndicesCount());

// 	// 	for (int i = 0; i < lCluster->GetControlPointIndicesCount(); ++i)
// 	// 	{
// 	// 		const auto lFindResult = fbxIndicesToOwnMap.find(lCluster->GetControlPointIndices()[i]);
// 	// 		assert(lFindResult != fbxIndicesToOwnMap.end());

// 	// 		lBoneInfluence.emplace_back
// 	// 		(
// 	// 			lFindResult->second, 
// 	// 			static_cast<GLfloat>(lCluster->GetControlPointWeights()[i])
// 	// 		);
// 	// 	}

// 	// 	lBoneToInfluenceMap.emplace(lLink, std::move(lBoneInfluence));
// 	// 	lBoneToClusterMap.emplace(lLink, lCluster);

// 	// 	// Is this the root bone?
// 	// 	if (lLink->GetParent() == lLink->GetScene()->GetRootNode())
// 	// 	{
// 	// 		lRootBone = lLink;
// 	// 		std::cerr << "\t\t" << lLink->GetName() << " is not the root node!\n";
// 	// 	}
// 	// 	else if (lLink->GetParent() == pFbxMesh->GetNode())
// 	// 	{
// 	// 		lRootBone = lLink;
// 	// 		std::cerr << "\t\t" << lLink->GetName() << " is the mesh node!\n";
// 	// 	}
// 	// 	else
// 	// 	{
// 	// 		std::cerr << "\t\t" << lLink->GetName() << " is neither the root nor the mesh node.\n";
// 	// 	}

// 	// }

// 	// assert(lRootBone);

// 	// buildBonesRecursive(lRootBone, -1, lIndexToBoneMap, lBoneToClusterMap);

// 	// assert(static_cast<Mesh::Bone::IndexType>(bones.size()) == lBoneCount);

// 	// for (Bone::IndexType i = 0; i < static_cast<Bone::IndexType>(bones.size()); ++i)
// 	// {
// 	// 	const auto r1 = lIndexToBoneMap.find(i);
// 	// 	assert(r1 != lIndexToBoneMap.end());
// 	// 	const auto r2 = lBoneToInfluenceMap.find(r1->second);
// 	// 	assert(r2 != lBoneToInfluenceMap.end());
// 	// 	const auto& lBoneInfluence = r2->second;
// 	// 	// const auto& lBoneInfluence = lBoneToInfluenceMap[lIndexToBoneMap[i]];
// 	// 	for (const auto& lPair : lBoneInfluence)
// 	// 	{
// 	// 		if (mJointIndices[lPair.first].x == -1)
// 	// 		{
// 	// 			mJointIndices[lPair.first].x = i;
// 	// 			mJointWeights[lPair.first].x = lPair.second;
// 	// 		}
// 	// 		else if (mJointIndices[lPair.first].y == -1)
// 	// 		{
// 	// 			mJointIndices[lPair.first].y = i;
// 	// 			mJointWeights[lPair.first].y = lPair.second;
// 	// 		}
// 	// 		else if (mJointIndices[lPair.first].z == -1)
// 	// 		{
// 	// 			mJointIndices[lPair.first].z = i;
// 	// 			mJointWeights[lPair.first].z = lPair.second;
// 	// 		}
// 	// 		else if (mJointIndices[lPair.first].w == -1)
// 	// 		{
// 	// 			mJointIndices[lPair.first].w = i;
// 	// 			mJointWeights[lPair.first].w = lPair.second;
// 	// 		}
// 	// 		else
// 	// 		{
// 	// 			std::cerr << "\tWARNING: Vertex " << lPair.first << " is influenced by more than 4 bones:\n";
// 	// 			std::cerr << "\t\t" << skeleton[mJointIndices[lPair.first].x].name << '\n';
// 	// 			std::cerr << "\t\t" << skeleton[mJointIndices[lPair.first].y].name << '\n';
// 	// 			std::cerr << "\t\t" << skeleton[mJointIndices[lPair.first].z].name << '\n';
// 	// 			std::cerr << "\t\t" << skeleton[mJointIndices[lPair.first].w].name << '\n';
// 	// 			std::cerr << "\t\t" << skeleton[i].name << '\n';
// 	// 		}
// 	// 	}
// 	// }
// }

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
	if (hasSkinning())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_BONE_IDS]);
		gtBufferData(GL_ARRAY_BUFFER, mJointIndices, lUsageHint);
		Mesh::vec4i::enableAttribute(GT_VERTEX_LAYOUT_SLOT_14);

		glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_BONE_WEIGHTS]);
		gtBufferData(GL_ARRAY_BUFFER, mJointWeights, lUsageHint);
		Mesh::vec4f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_15);
	}

	glBindVertexArray(mVertexArrayObjectAdjacencies);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_INDICES_ADJ]);
	gtBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicesAdjacent, lUsageHint);

	glBindBuffer(GL_ARRAY_BUFFER, mBuffer[GT_MESH_BUFFER_POSITIONS]);
	gtBufferData(GL_ARRAY_BUFFER, mPositions, lUsageHint);
	Mesh::vec3f::enableAttribute(GT_VERTEX_LAYOUT_SLOT_0);
}

} // namespace gintonic