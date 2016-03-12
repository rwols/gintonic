/**
 * @file mesh.hpp
 * @brief Defines the mesh base class component.
 * @author Raoul Wols
 */

#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "opengl/vertex_array_object.hpp"
#include "opengl/vector.hpp"
#include "Object.hpp"
#include "box3f.hpp"
#include "mat3f.hpp"
#include "mat4f.hpp"
#include "allocator.hpp"
#include "filesystem.hpp"
#include <vector>

namespace FBX
{
	class FbxVector2; // Forward declaration.
	class FbxVector4; // Forward declaration.
	class FbxMesh;    // Forward declaration.
} // namespace FBX

class aiMesh; // Forward declaration.

namespace gintonic {

/**
 * @brief Mesh component.
 * @details You can either draw the mesh instanced or non-intanced. When
 * drawing the mesh non-instanced, you should use the renderer for managing
 * the current modelview and modelviewprojection matrices. When rendering
 * the mesh instanced, you need to supply the matrices yourself.
 */
class Mesh : public Object<Mesh>
{
public:

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
		vec2f(const FBX::FbxVector2&);

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
		static void enable_attribute(const GLuint index) noexcept;
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
		vec3f(const FBX::FbxVector4& v);

		/// Constructor that takes an X, Y and Z value.
		vec3f(const GLfloat x, const GLfloat y, const GLfloat z);

		/// Equality comparison operator.
		bool operator == (const vec3f&) const noexcept;

		/// Inequality comparison operator.
		bool operator != (const vec3f&) const noexcept;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enable_attribute(const GLuint index) noexcept;
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
		vec4f(const FBX::FbxVector4&);

		/// Constructor that takes an X, Y, Z and W value.
		vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

		/// Equality comparison operator.
		bool operator == (const vec4f&) const noexcept;
	
		/// Inequality comparison operator.
		bool operator != (const vec4f&) const noexcept;

		/**
		 * @brief Enables this vector type in the current OpenGL array buffer.
		 * @details When an OpenGL array buffer is bound, normally you would
		 * call glVertexAttribPointer and glEnableAttribute. This static
		 * method does it for you so that one doesn't have to care about the
		 * correct arguments.
		 * 
		 * @param index The index.
		 */
		static void enable_attribute(const GLuint index) noexcept;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int /*version*/)
		{
			ar & x & y & z & w;
		}
	};

	struct LocalData
	{
		LocalData() = default;
		LocalData(const aiMesh*);
		LocalData(const FBX::FbxMesh*);

		std::vector<GLuint> mIndices;
		std::vector<Mesh::vec4f> mSlot0;
		std::vector<Mesh::vec4f> mSlot1;
		std::vector<Mesh::vec4f> mSlot2;

		template <class Archive> 
		void serialize(Archive& ar, const unsigned int /*version*/)
		{
			ar & mIndices & mSlot0 & mSlot1 & mSlot2;
		}
	};

	const box3f& getLocalBoundingBox() const noexcept
	{
		return mLocalBoundingBox;
	}

	/**
	 * @brief Default constructor.
	 */
	Mesh();

	/**
	 * @brief Constructs a mesh from an `aiMesh`.
	 * 
	 * @param [in] assimpMesh A constant pointer to an assimp mesh.
	 */
	Mesh(
		const aiMesh*                   assimpMesh);

	/**
	 * @brief Constructs a mesh from an `FbxMesh`.
	 * 
	 * @param [in] fbxMesh A constant pointer to an FBX mesh.
	 */
	Mesh(
		const FBX::FbxMesh*             fbxMesh);

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
	 * @brief Change this mesh according to the new `aiMesh`.
	 * 
	 * @param [in] assimpMesh A constant pointer to an assimp mesh.
	 */
	void set(
		const aiMesh*                   assimpMesh);

	/**
	 * @brief Change this mesh according to the new `FbxMesh`.
	 * 
	 * @param [in] fbxMesh A constant pointer to an FBX mesh.
	 */
	void set(
		const FBX::FbxMesh*             fbxMesh);

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
	 * @brief Draw the mesh.
	 */
	void draw() const noexcept;

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
		return mHasTangentsAndBitangents;
	}

	/**
	 * @brief Get the size of the indices array.
	 * @return The size of the indices array.
	 */
	inline GLsizei numIndices() const noexcept
	{
		return mNumIndices;
	}

protected:

	/// Constructor.
	inline Mesh(const MeshType t)
	: mMatrixBuffer(GL_DYNAMIC_DRAW)
	, mNormalMatrixBuffer(GL_DYNAMIC_DRAW)
	, m_type(t)
	{
		/* Empty on purpose. */
	}

private:

	void setupInstancedRenderingMatrices() noexcept;
	void computeLocalBoundingBoxFromPositionInformation(const std::vector<Mesh::vec4f>& position_XYZ_uv_X);

	opengl::vertex_array_object mVertexArrayObject;
	opengl::buffer_object_array<4> mBuffer;
	opengl::vector_array<GL_ARRAY_BUFFER, mat4f, 2> mMatrixBuffer;
	opengl::vector<GL_ARRAY_BUFFER, mat3f> mNormalMatrixBuffer;
	bool mHasTangentsAndBitangents;
	GLsizei mNumIndices;
	box3f mLocalBoundingBox;
	MeshType m_type;
};

} // namespace gintonic

#endif