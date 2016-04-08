/**
 * @file Mesh.hpp
 * @brief Defines the mesh base class component.
 * @author Raoul Wols
 */

#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/allocator.hpp"
#include "../Foundation/filesystem.hpp"
#include "../Foundation/Object.hpp"

#include "../Math/box3f.hpp"
#include "../Math/mat3f.hpp"
#include "../Math/mat4f.hpp"

#include "OpenGL/VertexArrayObject.hpp"
#include "OpenGL/Vector.hpp"

#include <vector>
#include <boost/serialization/base_object.hpp>

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

namespace gintonic {

/**
 * @brief Mesh component.
 * @details You can either draw the mesh instanced or non-intanced. When
 * drawing the mesh non-instanced, you should use the renderer for managing
 * the current modelview and modelviewprojection matrices. When rendering
 * the mesh instanced, you need to supply the matrices yourself.
 */
class Mesh : public Object<Mesh, std::string>
{
public:

	/**
	 * @deprecated
	 */
	enum MeshType
	{
		kStaticMesh,
		kDynamicMesh,
		kStandardMesh
	};


	/**
	 * @brief An additional vector class that doesn't use SSE types.
	 * @details You can use this class to build meshes internally.
	 */
	struct vec2f
	{
		/// The X-coordinate.
		GLfloat x;

		/// The Y-coordinate.
		GLfloat y;

		/// Default constructor.
		vec2f() = default;

		/// Takes an FbxVector2.
		vec2f(const FBXSDK_NAMESPACE::FbxVector2&);

		/// Constructor that takes an X and a Y value.
		vec2f(const GLfloat x, const GLfloat y);

		/// Equality comparison operator.
		bool operator == (const vec2f&) const noexcept;

		/// Inequality comparison operator.
		bool operator != (const vec2f&) const noexcept;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enableAttribute(const GLuint index) noexcept;
	};

	/**
	 * @brief An additional vector class that doesn't use SSE types.
	 * @details You can use this class to build meshes internally.
	 */
	struct vec3f
	{
		/// The X-coordinate.
		GLfloat x;

		/// The Y-coordinate.
		GLfloat y;

		/// The Z-coordinate.
		GLfloat z;

		/// Default constructor.
		vec3f() = default;

		/**
		 * @brief Constructor that takes an FbxVector4.
		 * @details Only the first three components of the FbxVector4 are
		 * used. The fourth component is ignored.
		 * 
		 * @param v The FbxVector4.
		 */
		vec3f(const FBXSDK_NAMESPACE::FbxVector4& v);

		/// Constructor that takes an X, Y and Z value.
		vec3f(const GLfloat x, const GLfloat y, const GLfloat z);

		/// Equality comparison operator.
		bool operator == (const vec3f&) const noexcept;

		/// Inequality comparison operator.
		bool operator != (const vec3f&) const noexcept;

		/// You can put these in a map.
		bool operator < (const vec3f&) const noexcept;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enableAttribute(const GLuint index) noexcept;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int /*version*/)
		{
			ar & x & y & z;
		}
	};

	/**
	 * @brief An additional vector class that doesn't use SSE types.
	 * @details You can use this class to build meshes internally.
	 */
	struct vec4f
	{
		/// The X-coordinate.
		GLfloat x;

		/// The Y-coordinate.
		GLfloat y;

		/// The Z-coordinate.
		GLfloat z;

		/// The W-coordinate.
		GLfloat w;

		/// Constructor.
		vec4f() = default;

		/// Constructor that takes an FbxVector4.
		vec4f(const FBXSDK_NAMESPACE::FbxVector4&);

		/// Constructor that takes an X, Y, Z and W value.
		vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

		/// Equality comparison operator.
		bool operator == (const vec4f&) const noexcept;
	
		/// Inequality comparison operator.
		bool operator != (const vec4f&) const noexcept;

		/// You can put these in a map.
		bool operator < (const vec4f&) const noexcept;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enableAttribute(const GLuint index) noexcept;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int /*version*/)
		{
			ar & x & y & z & w;
		}
	};

	/// Get the local bounding box.
	const box3f& getLocalBoundingBox() const noexcept
	{
		return mLocalBoundingBox;
	}

	/**
	 * @brief Default constructor.
	 */
	Mesh();

	/**
	 * @brief Constructs a mesh from an `FbxMesh`.
	 * 
	 * @param [in] fbxMesh A constant pointer to an FBX mesh.
	 */
	Mesh(
		const FBXSDK_NAMESPACE::FbxMesh*             fbxMesh);

	/**
	 * @brief Constructs a mesh from manual arrays.
	 * @details The arrays `position_XYZ_uv_X` and `normal_XYZ_uv_Y` must be of the same size.
	 * The values of the index array must refer to valid indices in the position and normal arrays.
	 * The index array can be of any size.
	 * @param [in] indices The index array.
	 * @param [in] position_XYZ_uv_X An array of four-dimensional vectors where the x,y,z coordinates
	 * are the position information for a vertex, and the w coordinate is the x coordinate of the
	 * texture coordinate information.
	 * @param [in] normal_XYZ_uv_Y An array of four-dimensional vectors where the x,y,z coordinates
	 * are the normal vector information for a vertex, and the w coordinate is the y coordinate of the
	 * texture coordinate information.
	 */
	Mesh(
		const std::vector<GLuint>&      indices, 
		const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
		const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y);

	/**
	 * @brief Constructs a mesh from manual arrays.
	 * @details The arrays `position_XYZ_uv_X`, `normal_XYZ_uv_Y` and `tangent_XYZ_handedness` must
	 * be of the same size.
	 * The values of the index array must refer to valid indices in the position and normal arrays.
	 * The index array can be of any size. The w-coordinate of the `tangent_XYZ_handedness` array
	 * is the *handedness*. This works as follows: When you have the normal vector, say `N`, and the
	 * tangent vector, say `T`, then to get the bitangent `B` vector you can simply calculate the cross
	 * product of `N` and `T`. But some modeling software systems use a reversed bitangent vector `-B`,
	 * so you must record the `+1` or `-1` information in this last w-coordinate. In a vertex shader,
	 * the bitangent vector is retrieved with a cross product of `N` and `T` and multiplied by the
	 * handedness.
	 * @param [in] indices The index array.
	 * @param [in] position_XYZ_uv_X An array of four-dimensional vectors where the x,y,z coordinates
	 * are the position information for a vertex, and the w coordinate is the x coordinate of the
	 * texture coordinate information.
	 * @param [in] normal_XYZ_uv_Y An array of four-dimensional vectors where the x,y,z coordinates
	 * are the normal vector information for a vertex, and the w coordinate is the y coordinate of the
	 * texture coordinate information.
	 * @param [in] tangent_XYZ_handedness An array of four-dimensional vectors where the x,y,z coordinates
	 * are the tangent vector information for a vertex, and the w coordinate is the *handedness* of the
	 * bitangent vector.
	 */
	Mesh(
		const std::vector<GLuint>&      indices, 
		const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
		const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y, 
		const std::vector<Mesh::vec4f>& tangent_XYZ_handedness);

	/**
	 * @brief Constructs a mesh from a serialized native mesh.
	 * 
	 * @param [in] nativeMeshFile A path to a native mesh file.
	 */
	Mesh(
		const boost::filesystem::path&  nativeMeshFile);

	/**
	 * @brief Change this mesh according to the new `FbxMesh`.
	 * 
	 * @param [in] fbxMesh A constant pointer to an FBX mesh.
	 */
	void set(
		const FBXSDK_NAMESPACE::FbxMesh*             fbxMesh);

	/**
	 * @brief Change this mesh according to manual arrays.
	 * @details The arrays `position_XYZ_uv_X` and `normal_XYZ_uv_Y` must be of the same size.
	 * The values of the index array must refer to valid indices in the position and normal arrays.
	 * The index array can be of any size.
	 * @param [in] indices The index array.
	 * @param [in] position_XYZ_uv_X An array of four-dimensional vectors where the x,y,z coordinates
	 * are the position information for a vertex, and the w coordinate is the x coordinate of the
	 * texture coordinate information.
	 * @param [in] normal_XYZ_uv_Y An array of four-dimensional vectors where the x,y,z coordinates
	 * are the normal vector information for a vertex, and the w coordinate is the y coordinate of the
	 * texture coordinate information.
	 */
	void set(
		const std::vector<GLuint>&      indices, 
		const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
		const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y);

	/**
	 * @brief Change this mesh according to manual arrays.
	 * @details The arrays `position_XYZ_uv_X`, `normal_XYZ_uv_Y` and `tangent_XYZ_handedness` must
	 * be of the same size.
	 * The values of the index array must refer to valid indices in the position and normal arrays.
	 * The index array can be of any size. The w-coordinate of the `tangent_XYZ_handedness` array
	 * is the *handedness*. This works as follows: When you have the normal vector, say `N`, and the
	 * tangent vector, say `T`, then to get the bitangent `B` vector you can simply calculate the cross
	 * product of `N` and `T`. But some modeling software systems use a reversed bitangent vector `-B`,
	 * so you must record the `+1` or `-1` information in this last w-coordinate. In a vertex shader,
	 * the bitangent vector is retrieved with a cross product of `N` and `T` and multiplied by the
	 * handedness.
	 * @param [in] indices The index array.
	 * @param [in] position_XYZ_uv_X An array of four-dimensional vectors where the x,y,z coordinates
	 * are the position information for a vertex, and the w coordinate is the x coordinate of the
	 * texture coordinate information.
	 * @param [in] normal_XYZ_uv_Y An array of four-dimensional vectors where the x,y,z coordinates
	 * are the normal vector information for a vertex, and the w coordinate is the y coordinate of the
	 * texture coordinate information.
	 * @param [in] tangent_XYZ_handedness An array of four-dimensional vectors where the x,y,z coordinates
	 * are the tangent vector information for a vertex, and the w coordinate is the *handedness* of the
	 * bitangent vector.
	 */
	void set(
		const std::vector<GLuint>&      indices, 
		const std::vector<Mesh::vec4f>& position_XYZ_uv_X, 
		const std::vector<Mesh::vec4f>& normal_XYZ_uv_Y, 
		const std::vector<Mesh::vec4f>& tangent_XYZ_handedness);

	/**
	 * @brief Constructs a mesh from a serialized native mesh.
	 * 
	 * @param [in] nativeMeshFile A path to a native mesh file.
	 */
	void set(
		const boost::filesystem::path&  nativeMeshFile);

	/**
	 * @brief Draw the mesh. Uses `GL_TRIANGLES` as draw mode.
	 */
	void draw() const noexcept;

	/**
	 * @brief Draw the mesh using the adjacency array. Uses
	 * `GL_TRIANGLES_ADJACENCY` as draw mode. Use this for silhouette
	 * detection algorithms. Only position information is streamed
	 * to the vertex shader, so a single vec3 with position information.
	 * You probably want to use a geometry shader to operate on the
	 * adjacency information.
	 */
	void drawAdjacent() const noexcept;

	/**
	 * @brief Draw the mesh instanced.
	 * @details For reach instance, you need to supply a PVM matrix, a VM
	 * matrix and an N matrix. You need to make sure that each of these
	 * arrays is the same size.
	 * 
	 * @param PVM_matrices An array of PVM matrices.
	 * @param VM_matrices An array of VM matrices.
	 * @param N_matrices An array of N matrices.
	 */
	void draw(
		const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
		const std::vector<mat4f, allocator<mat4f>>& VM_matrices,
		const std::vector<mat3f, allocator<mat3f>>& N_matrices);

	/**
	 * @brief Check wether this mesh has tangents and bitangents.
	 * @return True if the mesh has tangents and bitangents, false otherwise.
	 */
	inline bool hasTangentsAndBitangents() const noexcept
	{
		return !mTangent_XYZ_hand.empty();
	}

	/**
	 * @brief Get the size of the indices array.
	 * @return The size of the indices array.
	 */
	inline GLsizei numIndices() const noexcept
	{
		return static_cast<GLsizei>(mIndices.size());
	}

	inline GLsizei numIndicesAdjacent() const noexcept
	{
		return static_cast<GLsizei>(mIndicesAdjacent.size());
	}

	inline bool hasAdjacency() const noexcept
	{
		return numIndicesAdjacent() != 0;
	}

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

private:

	box3f mLocalBoundingBox;
	std::vector<GLuint> mIndices;
	std::vector<GLuint> mIndicesAdjacent;
	std::vector<Mesh::vec3f> mPositions;
	std::vector<Mesh::vec4f> mPosition_XYZ_uv_X;
	std::vector<Mesh::vec4f> mNormal_XYZ_uv_Y;
	std::vector<Mesh::vec4f> mTangent_XYZ_hand;

	OpenGL::VertexArrayObject mVertexArrayObject;
	OpenGL::VertexArrayObject mVertexArrayObjectAdjacencies;
	OpenGL::BufferObjectArray<6> mBuffer;
	OpenGL::VectorArray<GL_ARRAY_BUFFER, mat4f, 2> mMatrixBuffer;
	OpenGL::Vector<GL_ARRAY_BUFFER, mat3f> mNormalMatrixBuffer;

	void setupInstancedRenderingMatrices() noexcept;
	void computeLocalBoundingBoxFromPositionInformation(const std::vector<Mesh::vec4f>& position_XYZ_uv_X);
	void computeAdjacencyFromPositionInformation();
	
	void uploadData();

	friend boost::serialization::access;

	template <class Archive>
	void save(Archive& archive, const unsigned int /*version*/) const
	{
		archive & mLocalBoundingBox;
		archive & mIndices;
		archive & mIndicesAdjacent;
		archive & mPosition_XYZ_uv_X;
		archive & mNormal_XYZ_uv_Y;
		archive & mTangent_XYZ_hand;
		archive & mPositions;
	}

	template <class Archive>
	void load(Archive& archive, const unsigned int /*version*/)
	{
		archive & mLocalBoundingBox;
		archive & mIndices;
		archive & mIndicesAdjacent;
		archive & mPosition_XYZ_uv_X;
		archive & mNormal_XYZ_uv_Y;
		archive & mTangent_XYZ_hand;
		archive & mPositions;
		uploadData();
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

inline std::ostream& operator << (std::ostream& os, const Mesh::vec3f& v)
{
	return os << '[' << v.x << ", " << v.y << ", " << v.z << ", " << ']';
}

inline std::ostream& operator << (std::ostream& os, const Mesh::vec4f& v)
{
	return os << '[' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ']';
}

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Mesh, boost::serialization::track_always);
