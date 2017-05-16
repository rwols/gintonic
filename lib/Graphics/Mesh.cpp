#include "Graphics/Mesh.hpp"

#include "Foundation/exception.hpp"
#include "Foundation/tuple.hpp"

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
        else
            throw std::logic_error("Position indices are the same!");
    }

    bool operator<(const Edge& other) const noexcept
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

    bool operator<(const Triangle& other) const noexcept
    {
        return std::tie(indices[0], indices[1], indices[2]) <
               std::tie(other[0], other[1], other[2]);
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
        if (first == me)
            return second;
        else if (second == me)
            return first;
        else
            throw std::logic_error("getOther failed");
    }
};

// std::ostream& operator << (std::ostream& os, const NeighborPair&
// neighborPair)
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
void copyLayerElement(const LayerElement* layer, OutputIter iter)
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
bool getLayerElement(const LayerElement* layerElement, const int lPolygonID,
                     const int lVertexID, VectorType& r)
{
    if (!layerElement) return false;
    switch (layerElement->GetMappingMode())
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
            const auto lIndex2Element =
                layerElement->GetIndexArray().GetAt(lPolygonID);
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
        switch (layerElement->GetReferenceMode())
        {
        case FbxGeometryElement::eDirect:
        {
            r = layerElement->GetDirectArray().GetAt(lVertexID);
            break;
        }
        case FbxGeometryElement::eIndexToDirect:
        {
            const auto lIndex2Element =
                layerElement->GetIndexArray().GetAt(lVertexID);
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

namespace gintonic
{

Mesh::vec2f::vec2f(const GLfloat x, const GLfloat y) : x(x), y(y)
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
    glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec2f),
                          nullptr);
}

Mesh::vec3f::vec3f(const GLfloat x, const GLfloat y, const GLfloat z)
    : x(x), y(y), z(z)
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
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec3f),
                          nullptr);
}

Mesh::vec4f::vec4f(const GLfloat x, const GLfloat y, const GLfloat z,
                   const GLfloat w)
    : x(x), y(y), z(z), w(w)
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
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::vec4f),
                          nullptr);
}

Mesh::vec4i::vec4i(const GLint x, const GLint y, const GLint z, const GLint w)
    : x(x), y(y), z(z), w(w)
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
    : mMatrixBuffer(GL_DYNAMIC_DRAW), mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
    setupInstancedRenderingMatrices();
}

Mesh::Mesh(const boost::filesystem::path& filename)
    : mMatrixBuffer(GL_DYNAMIC_DRAW), mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
    setupInstancedRenderingMatrices();
    set(filename);
}

Mesh::Mesh(const std::vector<GLuint>& indices,
           const std::vector<Mesh::vec4f>& position_XYZ_uv_X,
           const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y)
    : mMatrixBuffer(GL_DYNAMIC_DRAW), mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
    setupInstancedRenderingMatrices();
    set(indices, position_XYZ_uv_X, normal_XYZ_uv_Y);
}

Mesh::Mesh(const std::vector<GLuint>& indices,
           const std::vector<Mesh::vec4f>& position_XYZ_uv_X,
           const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y,
           const std::vector<Mesh::vec4f>& tangent_XYZ_handedness)
    : mMatrixBuffer(GL_DYNAMIC_DRAW), mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
{
    setupInstancedRenderingMatrices();
    set(indices, position_XYZ_uv_X, normal_XYZ_uv_Y, tangent_XYZ_handedness);
}

void Mesh::set(const std::vector<GLuint>& indices,
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

void Mesh::set(const std::vector<GLuint>& indices,
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
    glDrawElements(GL_TRIANGLES_ADJACENCY, numIndicesAdjacent(),
                   GL_UNSIGNED_INT, nullptr);
}

void Mesh::draw(const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
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

    glDrawElementsInstanced(GL_TRIANGLES, numIndices(), GL_UNSIGNED_INT,
                            nullptr, mMatrixBuffer.size(0));
}

void Mesh::setupInstancedRenderingMatrices() noexcept
{
    glBindVertexArray(mVertexArrayObject);
    mMatrixBuffer.bind(0);
    for (GLuint i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(GT_VERTEX_LAYOUT_SLOT_3_4_5_6 + i);

        glVertexAttribPointer(GT_VERTEX_LAYOUT_SLOT_3_4_5_6 + i, 4, GL_FLOAT,
                              GL_FALSE, sizeof(mat4f),
                              (const GLvoid*)(sizeof(GLfloat) * i * 4));

        glVertexAttribDivisor(GT_VERTEX_LAYOUT_SLOT_3_4_5_6 + i, 1);
    }
    mMatrixBuffer.bind(1);
    for (GLuint i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(GT_VERTEX_LAYOUT_SLOT_7_8_9_10 + i);

        glVertexAttribPointer(GT_VERTEX_LAYOUT_SLOT_7_8_9_10 + i, 4, GL_FLOAT,
                              GL_FALSE, sizeof(mat4f),
                              (const GLvoid*)(sizeof(GLfloat) * i * 4));

        glVertexAttribDivisor(GT_VERTEX_LAYOUT_SLOT_7_8_9_10 + i, 1);
    }
    mNormalMatrixBuffer.bind();
    for (GLuint i = 0; i < 3; ++i)
    {
        glEnableVertexAttribArray(GT_VERTEX_LAYOUT_SLOT_11_12_13 + i);

        glVertexAttribPointer(GT_VERTEX_LAYOUT_SLOT_11_12_13 + i, 3, GL_FLOAT,
                              GL_FALSE, sizeof(mat3f),
                              (const GLvoid*)(sizeof(GLfloat) * i * 3));

        glVertexAttribDivisor(GT_VERTEX_LAYOUT_SLOT_11_12_13 + i, 1);
    }
}

void Mesh::computeLocalBoundingBoxFromPositionInformation(
    const std::vector<Mesh::vec4f>& position_XYZ_uv_X)
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
            const Mesh::vec3f lPosition(lPosition4.x, lPosition4.y,
                                        lPosition4.z);

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
