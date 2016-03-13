/**
 * @file vertices.hpp
 * @brief Defines various vertex types.
 * @author Raoul Wols
 */

#ifndef vertices_hpp
#define vertices_hpp

#include "../Math/vec2f.hpp"
#include "../Math/vec3f.hpp"
#include "../Math/vec4f.hpp"

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

//!@cond
// It's time to remove this set of macros in favor of the ones below.
#define GINTONIC_VERTEX_LAYOUT_POSITION 0
#define GINTONIC_VERTEX_LAYOUT_TEXCOORD 1
#define GINTONIC_VERTEX_LAYOUT_NORMAL 2
#define GINTONIC_VERTEX_LAYOUT_TANGENT 3
#define GINTONIC_VERTEX_LAYOUT_BITANGENT 4
#define GINTONIC_VERTEX_LAYOUT_VM_MATRIX 5 // 6 7 mat3x4
#define GINTONIC_VERTEX_LAYOUT_N_MATRIX 8 // 9 10 mat3x3
#define GINTONIC_VERTEX_LAYOUT_COLOR 5
//!@endcond

/**
 * @brief The standard attribute location of a vertex position.
 */
#define GT_VERTEX_LAYOUT_POSITION 0

/**
 * @brief The standard attribute location of a texture coordinate.
 */
#define GT_VERTEX_LAYOUT_TEXCOORD 1

/**
 * @brief The standard attribute location of a surface normal.
 */
#define GT_VERTEX_LAYOUT_NORMAL 2

/**
 * @brief The standard attribute location for the PVM matrix, for instanced
 * rendering.
 */
#define GT_VERTEX_LAYOUT_PVM_MATRIX 3 // 4 5 6

/**
 * @brief The standard attribute location for the VM matrix, for instanced
 * rendering.
 */
#define GT_VERTEX_LAYOUT_VM_MATRIX 7 // 8 9 10

/**
 * @brief The standard attribute location for the N matrix, for instanced
 * rendering.
 */
#define GT_VERTEX_LAYOUT_N_MATRIX 11 // 12 13

/**
 * @brief Here's a free attribute slot. OpenGL guarantees at least 16
 * attribute slots (though there can be more depending on the GPU).
 */
#define GT_VERTEX_LAYOUT_FREE_14 14

/**
 * @brief Here's a free attribute slot. OpenGL guarantees at least 16
 * attribute slots (though there can be more depending on the GPU).
 */
#define GT_VERTEX_LAYOUT_FREE_15 15

namespace FBX
{
	class FbxMesh; // Forward declaration.
	class FbxVector4; // Forward declaration.
	class FbxVector2; // Forward declaration.
	class FbxColor; // Forward declaration.
}

namespace gintonic {
namespace opengl {

/**
 * @brief Vertex with positions.
 */
struct vertex_P
{
	float position[3]; //!< Position.

	/// Default constructor.
	vertex_P() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 */
	vertex_P(const float px, const float py, const float pz);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 */
	vertex_P(const vec3f& p);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_P(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_P(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, 
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_P(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vp".
	 */
	inline static const char* extension() noexcept
	{
		return ".vp";
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
	}

	/**
	 * @brief Equality comparison operator.
	 * @details Two vertices compare equal when all of their datamembers
	 * compare equal (exactly equal, not "almost equal" for floating point
	 * numbers).
	 * 
	 * @param other Another vertex.
	 * @return True if the vertices compare exactly equal, false otherwise.
	 */
	bool operator == (const vertex_P& other) const noexcept;

	/**
	 * @brief Inequality comparison operator.
	 * @details Two vertices compare equal when all of their datamembers
	 * compare equal (exactly equal, not "almost equal" for floating point
	 * numbers).
	 * 
	 * @param other Another vertex.
	 * @return False if the vertices compare exactly equal, true otherwise.
	 */
	inline bool operator != (const vertex_P& other) const 
		noexcept
	{
		return !operator==(other);
	}
};

/**
 * @brief Vertex with positions and colors.
 */
struct vertex_PC
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.

	/// Default constructor.
	vertex_PC() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param cx The X-coordinate of the color.
	 * @param cy The Y-coordinate of the color.
	 * @param cz The Z-coordinate of the color.
	 * @param cw The W-coordinate of the color.
	 */
	vertex_PC(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param c The color.
	 */
	vertex_PC(const vec3f& p, const vec4f& c);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PC(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PC(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, 
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PC(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpc".
	 */
	inline static const char* extension() noexcept
	{
		return ".vpc";
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & color;
	}
};

/**
 * @brief Vertex with positions and texture coordinates.
 */
struct vertex_PU
{
	float position[3]; //!< Position.
	float uv[2]; //!< Texture UVs.

	/// Default constructor.
	vertex_PU() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 */
	vertex_PU(const float px, const float py, const float pz,
		const float ux, const float uy);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param u The texture coordinates.
	 */
	vertex_PU(const vec3f& p, const vec2f& u);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PU(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PU(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, 
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PU(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpu".
	 */
	inline static const char* extension() noexcept
	{
		return ".vpu";
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & uv;
	}
};

/**
 * @brief Vertex with positions and normals.
 */
struct vertex_PN
{
	float position[3]; //!< Position.
	float normal[3]; //!< Normal.

	/// Default constructor.
	vertex_PN() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param nx The X-coordinate of the normal.
	 * @param ny The Y-coordinate of the normal.
	 * @param nz The Z-coordinate of the normal.
	 */
	vertex_PN(const float px, const float py, const float pz,
		const float nx, const float ny, const float nz);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param n The normal.
	 */
	vertex_PN(const vec3f& p, const vec3f& n);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PN(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, 
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PN(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".pn".
	 */
	inline static const char* extension() noexcept
	{
		return ".pn";
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & normal;
	}
};

/**
 * @brief Vertex with positions, colors and texture coordinates.
 */
struct vertex_PCU
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.

	/// Default constructor.
	vertex_PCU() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param cx The X-coordinate of the color.
	 * @param cy The Y-coordinate of the color.
	 * @param cz The Z-coordinate of the color.
	 * @param cw The W-coordinate of the color.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 */
	vertex_PCU(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 */
	vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PCU(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n,
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PCU(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpcu".
	 */
	inline static const char* extension() noexcept 
	{ 
		return ".vpcu"; 
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
	}
};

/**
 * @brief Vertex with positions, colors, texture coordinates and normals.
 */
struct vertex_PCUN
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.

	/// Default constructor.
	vertex_PCUN() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param cx The X-coordinate of the color.
	 * @param cy The Y-coordinate of the color.
	 * @param cz The Z-coordinate of the color.
	 * @param cw The W-coordinate of the color.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 * @param nx The X-coordinate of the normal.
	 * @param ny The Y-coordinate of the normal.
	 * @param nz The Z-coordinate of the normal.
	 */
	vertex_PCUN(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy,
		const float nx, const float ny, const float nz);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 */
	vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, 
		const vec3f& n);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PCUN(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, 
		const vec3f& n, const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PCUN(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpcun".
	 */
	inline static const char* extension() noexcept 
	{ 
		return ".vpcun"; 
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & normal;
	}
};

/**
 * @brief Vertex with positions, texture coordinates and normals.
 */
struct vertex_PUN
{
	float position[3]; //!< Position.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.

	/// Default constructor.
	vertex_PUN() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 * @param nx The X-coordinate of the normal.
	 * @param ny The Y-coordinate of the normal.
	 * @param nz The Z-coordinate of the normal.
	 */
	vertex_PUN(const float px, const float py, const float pz,
		const float ux, const float uy,
		const float nx, const float ny, const float nz);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 */
	vertex_PUN(const vec3f& p, const vec2f& u, const vec3f& n);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PUN(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n,
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PUN(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpun".
	 */
	inline static const char* extension() noexcept 
	{ 
		return ".vpun"; 
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & uv;
		archive & normal;
	}
};

/**
 * @brief Vertex with positions, colors, texture coordinates, normals,
 * tangents and bitangents.
 */
struct vertex_PCUNTB
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.
	float tangent[3]; //!< Tangent.
	float bitangent[3]; //!< Bitangent.

	/// Default constructor.
	vertex_PCUNTB() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param cx The X-coordinate of the color.
	 * @param cy The Y-coordinate of the color.
	 * @param cz The Z-coordinate of the color.
	 * @param cw The W-coordinate of the color.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 * @param nx The X-coordinate of the normal.
	 * @param ny The Y-coordinate of the normal.
	 * @param nz The Z-coordinate of the normal.
	 * @param tx The X-coordinate of the tangent.
	 * @param ty The Y-coordinate of the tangent.
	 * @param tz The Z-coordinate of the tangent.
	 * @param bx The X-coordinate of the bitangent.
	 * @param by The Y-coordinate of the bitangent.
	 * @param bz The Z-coordinate of the bitangent.
	 */
	vertex_PCUNTB(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy,
		const float nx, const float ny, const float nz,
		const float tx, const float ty, const float tz,
		const float bx, const float by, const float bz);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PCUNTB(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PCUNTB(const vec3f& p, const vec4f& c, const vec2f& u, 
		const vec3f& n, const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PCUNTB(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpcuntb".
	 */
	inline static const char* extension() noexcept 
	{ 
		return ".vpcuntb"; 
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & normal;
		archive & tangent;
		archive & bitangent;
	}
};

/**
 * @brief Vertex with positions, texture coordinates, normals, tangents and 
 * bitangents.
 */
struct vertex_PUNTB
{
	float position[3]; //!< Position.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.
	float tangent[3]; //!< Tangent.
	float bitangent[3]; //!< Bitangent.

	/// Default constructor.
	vertex_PUNTB() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 * @param nx The X-coordinate of the normal.
	 * @param ny The Y-coordinate of the normal.
	 * @param nz The Z-coordinate of the normal.
	 * @param tx The X-coordinate of the tangent.
	 * @param ty The Y-coordinate of the tangent.
	 * @param tz The Z-coordinate of the tangent.
	 * @param bx The X-coordinate of the bitangent.
	 * @param by The Y-coordinate of the bitangent.
	 * @param bz The Z-coordinate of the bitangent.
	 */
	vertex_PUNTB(const float px, const float py, const float pz,
		const float ux, const float uy,
		const float nx, const float ny, const float nz,
		const float tx, const float ty, const float tz,
		const float bx, const float by, const float bz);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PUNTB(const vec3f& p, const vec2f& u, const vec3f& n, 
		const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * 
	 * @details The constructor takes the mesh, gets the layer of the mesh,
	 * and then fetches the index at the layer. It collects the vertex data
	 * relevant for this vertex type.
	 * 
	 * @param pMesh Pointer to an FbxMesh.
	 * @param i The index in the direct array.
	 * @param layer The layer.
	 */
	vertex_PUNTB(FBX::FbxMesh const*const pMesh, const std::size_t i, 
		const std::size_t layer);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PUNTB(const vec3f& p, const vec4f& c, const vec2f& u, 
		const vec3f& n, const vec3f& t, const vec3f& b);

	/**
	 * @brief Constructor.
	 * @details Some vertex types do not use all the information given via the
	 * constructor. Some arguments may be ignored, depending on if the vertex
	 * needs this information.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param n The normal.
	 * @param t The tangent.
	 * @param b The bitangent.
	 */
	vertex_PUNTB(const FBX::FbxVector4& p, const FBX::FbxColor& c, 
		const FBX::FbxVector2& u, const FBX::FbxVector4& n, 
		const FBX::FbxVector4& t, const FBX::FbxVector4& b);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpuntb".
	 */
	inline static const char* extension() noexcept 
	{ 
		return ".vpuntb"; 
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & uv;
		archive & normal;
		archive & tangent;
		archive & bitangent;
	}
};

/**
 * @brief Vertex with positions, colors, texture coordinates, shift and gamma.
 */
struct vertex_PCUsg
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	float shift; //!< Shift along x.
	float gamma; //!< Color gamma correction.

	/// Default constructor.
	vertex_PCUsg() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The Z-coordinate of the position.
	 * @param cx The X-coordinate of the color.
	 * @param cy The Y-coordinate of the color.
	 * @param cz The Z-coordinate of the color.
	 * @param cw The W-coordinate of the color.
	 * @param ux The X-coordinate of the texture coordinate.
	 * @param uy The Y-coordinate of the texture coordinate.
	 * @param shift_value The shift value.
	 * @param gamma_value The gamma value.
	 */
	vertex_PCUsg(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy,
		const float shift_value,
		const float gamma_value);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The position.
	 * @param c The color.
	 * @param u The texture coordinates.
	 * @param shift_value The shift value.
	 * @param gamma_value The gamma value.
	 */
	vertex_PCUsg(const vec3f& p, const vec4f& c, const vec2f& u, 
		const float shift_value, const float gamma_value);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".vpcusg".
	 */
	inline static const char* extension() noexcept
	{ 
		return ".vpcusg"; 
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & shift;
		archive & gamma;
	}
};

/**
 * @brief Vertex with packed positions and texture coordinates.
 */
struct vertex_text2d
{
	float pos_and_texcoord[4]; //!< Packed position and texture coordinates.

	/// Default constructor.
	vertex_text2d() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param px The X-coordinate of the position.
	 * @param py The Y-coordinate of the position.
	 * @param pz The X-coordinate of the texture coordinates.
	 * @param pw The Y-coordinate of the texture coordinates.
	 */
	vertex_text2d(const float px, const float py, const float pz, 
		const float pw);

	/**
	 * @brief Constructor.
	 * 
	 * @param p The packed position and texture coordinates. The X and Y
	 * coordinates should be the position, the Z and W values should be the
	 * texture coordinates.
	 */
	vertex_text2d(const vec4f& p);

	/**
	 * @brief Constructor.
	 * 
	 * @param pos The position.
	 * @param texcoord The texture coordinates.
	 */
	vertex_text2d(const vec2f& pos, const vec2f& texcoord);

	/**
	 * @brief Enable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void enable_attributes() noexcept;

	/**
	 * @brief Disable the vertex attributes in the bound OpenGL array buffer.
	 */
	static void disable_attributes() noexcept;

	/**
	 * @brief The default filename extension for this type of vertex.
	 * @return Always returns the string literal ".text2d".
	 */
	inline static const char* extension() noexcept
	{
		return ".text2d";
	}

	/**
	 * @brief (De)serialization support.
	 * @details You don't use this method directly, but rather use the
	 * boost::serialization mechanism.
	 * 
	 * @tparam Archive The type of the archive.
	 * @param archive The input/output archive.
	 * @param version The version number.
	 */
	template <class Archive> void serialize(Archive& archive, 
		const unsigned int version)
	{
		archive & pos_and_texcoord;
	}
};

/// Output stream support for vertex_P.
std::ostream& operator << (std::ostream&, const vertex_P&);

/// Output stream support for vertex_PC.
std::ostream& operator << (std::ostream&, const vertex_PC&);

/// Output stream support for vertex_PU.
std::ostream& operator << (std::ostream&, const vertex_PU&);

/// Output stream support for vertex_PCU.
std::ostream& operator << (std::ostream&, const vertex_PCU&);

/// Output stream support for vertex_PUN.
std::ostream& operator << (std::ostream&, const vertex_PUN&);

/// Output stream support for vertex_PCUN.
std::ostream& operator << (std::ostream&, const vertex_PCUN&);

/// Output stream support for vertex_PUNTB.
std::ostream& operator << (std::ostream&, const vertex_PUNTB&);

/// Output stream support for vertex_PCUNTB.
std::ostream& operator << (std::ostream&, const vertex_PCUNTB&);

/// Output stream support for vertex_PCUsg.
std::ostream& operator << (std::ostream&, const vertex_PCUsg&);

/// Output stream support for vertex_text2d.
std::ostream& operator << (std::ostream&, const vertex_text2d&);

/**
 * @brief Test wether a type has a method called "enable_attributes".
 * 
 * @tparam T The type to test.
 */
template <typename T>
class has_enable_attributes_method
{
	typedef char one;
	typedef long two;

	template <typename C> static one test(decltype(&C::enable_attributes));
	template <typename C> static two test(...);

public:

	/**
	 * @brief Get the result of the test using the enum value.
	 */
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

} // end of namespace opengl
} // end of namespace gintonic

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#endif