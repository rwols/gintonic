/*!
\file vertices.hpp
\author Raoul Wols
\date 17/09/2014
\brief A collection of vertex classes.
*/

#ifndef vertices_hpp
#define vertices_hpp

// // Include the FBX SDK.
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wgnu"
// #pragma clang diagnostic ignored "-Wextra-semi"
// #define FBXSDK_NEW_API
// #include <fbxsdk.h>
// #pragma clang diagnostic pop

#include "opengl.hpp"
// #include "Serializable.hpp"

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

namespace gintonic {
namespace opengl {

/*!
\brief Vertex with position.
\sa vertex_PC
\sa vertex_PU
\sa vertex_PCU
\sa vertex_PCUN
\sa vertex_PUN
\sa vertex_PCUNTB
\sa vertex_PUNTB
*/
template <typename T> struct vertex_P
{
	typedef T float_type;
	T position[3]; //!< Position.

	vertex_P() {}
	vertex_P(const T px, const T py, const T pz)
	{
		position[0] = px; position[1] = py; position[2] = pz;
	}
	vertex_P(const vec3f& p)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
	}
	vertex_P(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
	}
	vertex_P(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = p[0];
		position[1] = p[1];
		position[2] = p[2];
	}


	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_P), (const GLvoid*)offsetof(vertex_P, position));
		glEnableVertexAttribArray(0);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vp"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
	}
	bool operator == (const vertex_P& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return position[0] == other.position[0] && position[1] == other.position[1]
			&& position[2] == other.position[2];
	}

	inline bool operator != (const vertex_P& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}
};

/*!
\brief Vertex with position and color.
\sa vertex_P
\sa vertex_PU
\sa vertex_PCU
\sa vertex_PCUN
\sa vertex_PUN
\sa vertex_PCUNTB
\sa vertex_PUNTB
*/
template <typename T> struct vertex_PC
{
	typedef T float_type;
	T position[3]; //!< Position.
	T color[4]; //!< Color.

	vertex_PC() {}
	vertex_PC(const T px, const T py, const T pz,
		const T cx, const T cy, const T cz, const T cw)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
	}
	vertex_PC(const vec3f& p, const vec4f& c)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		color[0] = c[0]; color[1] = c[1]; color[2] = c[2]; color[3] = c[3];
	}
	vertex_PC(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0] = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1] = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2] = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		auto fbx_color_layer = pMesh->GetLayer(layer)->GetVertexColors();
		auto fbx_color = fbx_color_layer->GetDirectArray()[fbx_color_layer->GetIndexArray()[i]];
		color[0] = fbx_color[0];
		color[1] = fbx_color[1];
		color[2] = fbx_color[2];
		color[3] = fbx_color[3];
		// color[0]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
		// color[1]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
		// color[2]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
		// color[3]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
	}
	vertex_PC(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = p[0];
		position[1] = p[1];
		position[2] = p[2];
		color[0] = c[0];
		color[1] = c[1];
		color[2] = c[2];
		color[3] = c[3];
	}


	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PC), (const GLvoid*)offsetof(vertex_PC, position));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PC), (const GLvoid*)offsetof(vertex_PC, color));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpc"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
	}
};

/*!
\brief Vertex with position and texture UVs.
\sa vertex_P
\sa vertex_PC
\sa vertex_PCU
\sa vertex_PCUN
\sa vertex_PUN
\sa vertex_PCUNTB
\sa vertex_PUNTB
*/
template <typename T> struct vertex_PU
{
	typedef T float_type;
	T position[3]; //!< Position.
	T uv[2]; //!< Texture UVs.

	vertex_PU() {}
	vertex_PU(const T px, const T py, const T pz,
		const T ux, const T uy)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		uv[0] = ux; uv[1] = uy;
	}
	vertex_PU(const vec3f& p, const vec2f& u)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		uv[0] = u[0]; uv[1] = u[1];
	}
	vertex_PU(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0] = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1] = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2] = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		uv[0]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
		uv[1]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
	}
	vertex_PU(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = static_cast<float_type>(p[0]);
		position[1] = static_cast<float_type>(p[1]);
		position[2] = static_cast<float_type>(p[2]);
		uv[0] = static_cast<float_type>(u[0]);
		uv[1] = static_cast<float_type>(u[1]);
	}

	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PU), (const GLvoid*)offsetof(vertex_PU, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PU), (const GLvoid*)offsetof(vertex_PU, uv));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpu"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & uv;
	}
};

/*!
\brief Vertex with position, color and texture UVs.
\sa vertex_P
\sa vertex_PC
\sa vertex_PU
\sa vertex_PCUN
\sa vertex_PUN
\sa vertex_PCUNTB
\sa vertex_PUNTB
*/
template <typename T> struct vertex_PCU
{
	typedef T float_type;
	T position[3]; //!< Position.
	T color[4]; //!< Color.
	T uv[2]; //!< Texture UVs.
	vertex_PCU() {}
	vertex_PCU(const T px, const T py, const T pz,
		const T cx, const T cy, const T cz, const T cw,
		const T ux, const T uy)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
		uv[0] = ux; uv[1] = uy;
	}
	vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		color[0] = c[0]; color[1] = c[1]; color[2] = c[2]; color[3] = c[3];
		uv[0] = u[0]; uv[1] = u[1];
	}
	vertex_PCU(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0] = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1] = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2] = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		color[0]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
		color[1]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
		color[2]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
		color[3]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
		uv[0]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
		uv[1]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
	}
	vertex_PCU(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = static_cast<float_type>(p[0]);
		position[1] = static_cast<float_type>(p[1]);
		position[2] = static_cast<float_type>(p[2]);
		color[0] = static_cast<float_type>(c[0]);
		color[1] = static_cast<float_type>(c[1]);
		color[2] = static_cast<float_type>(c[2]);
		color[3] = static_cast<float_type>(c[3]);
		uv[0] = static_cast<float_type>(u[0]);
		uv[1] = static_cast<float_type>(u[1]);
	}

	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCU), (const GLvoid*)offsetof(vertex_PCU, position));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCU), (const GLvoid*)offsetof(vertex_PCU, color));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCU), (const GLvoid*)offsetof(vertex_PCU, uv));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpcu"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
	}
};

/*!
\brief Vertex with position, color, texture UVs and normal.
\sa vertex_P
\sa vertex_PC
\sa vertex_PU
\sa vertex_PCU
\sa vertex_PUN
\sa vertex_PCUNTB
\sa vertex_PUNTB
*/
template <typename T> struct vertex_PCUN
{
	typedef T float_type;
	T position[3]; //!< Position.
	T color[4]; //!< Color.
	T uv[2]; //!< Texture UVs.
	T normal[3]; //!< Normal.
	vertex_PCUN() {}
	vertex_PCUN(const T px, const T py, const T pz,
		const T cx, const T cy, const T cz, const T cw,
		const T ux, const T uy,
		const T nx, const T ny, const T nz)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
		uv[0] = ux; uv[1] = uy;
		normal[0] = nx; normal[1] = ny; normal[2] = nz;
	}
	vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		color[0] = c[0]; color[1] = c[1]; color[2] = c[2]; color[3] = c[3];
		uv[0] = u[0]; uv[1] = u[1];
		normal[0] = n[0]; normal[1] = n[1]; normal[2] = n[2];
	}
	vertex_PCUN(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0] = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1] = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2] = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		color[0]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
		color[1]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
		color[2]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
		color[3]    = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
		uv[0]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
		uv[1]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
		normal[0]   = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
		normal[1]   = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
		normal[2]   = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
	}
	vertex_PCUN(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = static_cast<float_type>(p[0]);
		position[1] = static_cast<float_type>(p[1]);
		position[2] = static_cast<float_type>(p[2]);
		color[0] = static_cast<float_type>(c[0]);
		color[1] = static_cast<float_type>(c[1]);
		color[2] = static_cast<float_type>(c[2]);
		color[3] = static_cast<float_type>(c[3]);
		uv[0] = static_cast<float_type>(u[0]);
		uv[1] = static_cast<float_type>(u[1]);
		normal[0] = static_cast<float_type>(n[0]);
		normal[1] = static_cast<float_type>(n[1]);
		normal[2] = static_cast<float_type>(n[2]);
	}
	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, position));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, color));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, uv));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, normal));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpcun"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & normal;
	}
};

/*!
\brief Vertex with position, texture UVs and normal.
\sa vertex_P
\sa vertex_PC
\sa vertex_PU
\sa vertex_PCU
\sa vertex_PCUN
\sa vertex_PCUNTB
\sa vertex_PUNTB
*/
template <typename T> struct vertex_PUN
{
	typedef T float_type;
	T position[3]; //!< Position.
	T uv[2]; //!< Texture UVs.
	T normal[3]; //!< Normal.
	vertex_PUN() {}
	vertex_PUN(const T px, const T py, const T pz,
		const T ux, const T uy,
		const T nx, const T ny, const T nz)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		uv[0] = ux; uv[1] = uy;
		normal[0] = nx; normal[1] = ny; normal[2] = nz;
	}
	vertex_PUN(const vec3f& p, const vec2f& u, const vec3f& n)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		uv[0] = u[0]; uv[1] = u[1];
		normal[0] = n[0]; normal[1] = n[1]; normal[2] = n[2];
	}
	vertex_PUN(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		uv[0]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
		uv[1]       = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
		normal[0]   = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
		normal[1]   = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
		normal[2]   = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
	}
	vertex_PUN(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = static_cast<float_type>(p[0]);
		position[1] = static_cast<float_type>(p[1]);
		position[2] = static_cast<float_type>(p[2]);
		uv[0] = static_cast<float_type>(u[0]);
		uv[1] = static_cast<float_type>(u[1]);
		normal[0] = static_cast<float_type>(n[0]);
		normal[1] = static_cast<float_type>(n[1]);
		normal[2] = static_cast<float_type>(n[2]);
	}
	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUN), (const GLvoid*)offsetof(vertex_PUN, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PUN), (const GLvoid*)offsetof(vertex_PUN, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUN), (const GLvoid*)offsetof(vertex_PUN, normal));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpun"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & uv;
		archive & normal;
	}
};

/*!
\brief Vertex with position, color, texture UVs, normal, tangent and bitangent.
\sa vertex_P
\sa vertex_PC
\sa vertex_PU
\sa vertex_PCU
\sa vertex_PCUN
\sa vertex_PUN
\sa vertex_PUNTB
*/
template <typename T> struct vertex_PCUNTB
{
	typedef T float_type;
	T position[3]; //!< Position.
	T color[4]; //!< Color.
	T uv[2]; //!< Texture UVs.
	T normal[3]; //!< Normal.
	T tangent[3]; //!< Tangent.
	T bitangent[3]; //!< Bitangent.

	vertex_PCUNTB() {}
	vertex_PCUNTB(const T px, const T py, const T pz,
		const T cx, const T cy, const T cz, const T cw,
		const T ux, const T uy,
		const T nx, const T ny, const T nz,
		const T tx, const T ty, const T tz,
		const T bx, const T by, const T bz)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
		uv[0] = ux; uv[1] = uy;
		normal[0] = nx; normal[1] = ny; normal[2] = nz;
		tangent[0] = tx; tangent[1] = ty; tangent[2] = tz;
		bitangent[0] = bx; bitangent[1] = by; tangent[2] = bz;
	}
	vertex_PCUNTB(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f b)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		color[0] = c[0]; color[1] = c[1]; color[2] = c[2]; color[3] = c[3];
		uv[0] = u[0]; uv[1] = u[1];
		normal[0] = n[0]; normal[1] = n[1]; normal[2] = n[2];
		tangent[0] = t[0]; tangent[1] = t[1]; tangent[2] = t[2];
		bitangent[0] = b[0]; bitangent[1] = b[1]; bitangent[2] = b[2];
	}
	vertex_PCUNTB(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		color[0]     = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
		color[1]     = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
		color[2]     = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
		color[3]     = static_cast<float_type>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
		uv[0]        = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
		uv[1]        = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
		normal[0]    = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
		normal[1]    = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
		normal[2]    = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
		tangent[0]   = static_cast<float_type>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][0]);
		tangent[1]   = static_cast<float_type>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][1]);
		tangent[2]   = static_cast<float_type>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][2]);
		bitangent[0] = static_cast<float_type>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][0]);
		bitangent[1] = static_cast<float_type>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][1]);
		bitangent[2] = static_cast<float_type>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][2]);
	}
	vertex_PCUNTB(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = static_cast<float_type>(p[0]);
		position[1] = static_cast<float_type>(p[1]);
		position[2] = static_cast<float_type>(p[2]);
		color[0] = static_cast<float_type>(c[0]);
		color[1] = static_cast<float_type>(c[1]);
		color[2] = static_cast<float_type>(c[2]);
		color[3] = static_cast<float_type>(c[3]);
		uv[0] = static_cast<float_type>(u[0]);
		uv[1] = static_cast<float_type>(u[1]);
		normal[0] = static_cast<float_type>(n[0]);
		normal[1] = static_cast<float_type>(n[1]);
		normal[2] = static_cast<float_type>(n[2]);
		tangent[0] = static_cast<float_type>(t[0]);
		tangent[1] = static_cast<float_type>(t[1]);
		tangent[2] = static_cast<float_type>(t[2]);
		bitangent[0] = static_cast<float_type>(b[0]);
		bitangent[1] = static_cast<float_type>(b[1]);
		bitangent[2] = static_cast<float_type>(b[2]);
	}
	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, position));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, color));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, uv));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, normal));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, tangent));
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, bitangent));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpcuntb"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & normal;
		archive & tangent;
		archive & bitangent;
	}
};

/*!
\brief Vertex with position, texture UVs, normal, tangent and bitangent.
\sa vertex_P
\sa vertex_PC
\sa vertex_PU
\sa vertex_PCU
\sa vertex_PCUN
\sa vertex_PUN
\sa vertex_PCUNTB
*/
template <typename T> struct vertex_PUNTB
{
	typedef T float_type;
	T position[3]; //!< Position.
	T uv[2]; //!< Texture UVs.
	T normal[3]; //!< Normal.
	T tangent[3]; //!< Tangent.
	T bitangent[3]; //!< Bitangent.
	vertex_PUNTB() {}
	vertex_PUNTB(const T px, const T py, const T pz,
		const T ux, const T uy,
		const T nx, const T ny, const T nz,
		const T tx, const T ty, const T tz,
		const T bx, const T by, const T bz)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		uv[0] = ux; uv[1] = uy;
		normal[0] = nx; normal[1] = ny; normal[2] = nz;
		tangent[0] = tx; tangent[1] = ty; tangent[2] = tz;
		bitangent[0] = bx; bitangent[1] = by; tangent[2] = bz;
	}
	vertex_PUNTB(const vec3f& p, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f b)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		uv[0] = u[0]; uv[1] = u[1];
		normal[0] = n[0]; normal[1] = n[1]; normal[2] = n[2];
		tangent[0] = t[0]; tangent[1] = t[1]; tangent[2] = t[2];
		bitangent[0] = b[0]; bitangent[1] = b[1]; bitangent[2] = b[2];
	}
	vertex_PUNTB(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
	{
		position[0]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[0]);
		position[1]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[1]);
		position[2]  = static_cast<float_type>(pMesh->GetControlPointAt(i)[2]);
		uv[0]        = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
		uv[1]        = static_cast<float_type>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
		normal[0]    = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
		normal[1]    = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
		normal[2]    = static_cast<float_type>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
		tangent[0]   = static_cast<float_type>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][0]);
		tangent[1]   = static_cast<float_type>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][1]);
		tangent[2]   = static_cast<float_type>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][2]);
		bitangent[0] = static_cast<float_type>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][0]);
		bitangent[1] = static_cast<float_type>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][1]);
		bitangent[2] = static_cast<float_type>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][2]);
	}
	vertex_PUNTB(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
	{
		position[0] = static_cast<float_type>(p[0]);
		position[1] = static_cast<float_type>(p[1]);
		position[2] = static_cast<float_type>(p[2]);
		uv[0] = static_cast<float_type>(u[0]);
		uv[1] = static_cast<float_type>(u[1]);
		normal[0] = static_cast<float_type>(n[0]);
		normal[1] = static_cast<float_type>(n[1]);
		normal[2] = static_cast<float_type>(n[2]);
		tangent[0] = static_cast<float_type>(t[0]);
		tangent[1] = static_cast<float_type>(t[1]);
		tangent[2] = static_cast<float_type>(t[2]);
		bitangent[0] = static_cast<float_type>(b[0]);
		bitangent[1] = static_cast<float_type>(b[1]);
		bitangent[2] = static_cast<float_type>(b[2]);
	}
	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, normal));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, tangent));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, bitangent));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpuntb"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & uv;
		archive & normal;
		archive & tangent;
		archive & bitangent;
	}
};

/**
 * Glyph vertex structure
 */
template <typename T> struct vertex_PCUsg
{
	typedef T float_type;
	T position[3]; //!< Position.
	T color[4]; //!< Color.
	T uv[2]; //!< Texture UVs.
	T shift; //!< Shift along x.
	T gamma; //!< Color gamma correction.
	vertex_PCUsg() {}
	vertex_PCUsg(const T px, const T py, const T pz,
		const T cx, const T cy, const T cz, const T cw,
		const T ux, const T uy,
		const T shift_value,
		const T gamma_value)
	{
		position[0] = px; position[1] = py; position[2] = pz;
		color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
		uv[0] = ux; uv[1] = uy;
		shift = shift_value;
		gamma = gamma_value;
	}
	vertex_PCUsg(const vec3f& p, const vec4f& c, const vec2f& u, const T shift_value, const T gamma_value)
	{
		position[0] = p[0]; position[1] = p[1]; position[2] = p[2];
		color[0] = c[0]; color[1] = c[1]; color[2] = c[2]; color[3] = c[3];
		uv[0] = u[0]; uv[1] = u[1];
		shift = shift_value;
		gamma = gamma_value;
	}
	static void EnableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUsg), (const GLvoid*)offsetof(vertex_PCUsg, position));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUsg), (const GLvoid*)offsetof(vertex_PCUsg, color));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUsg), (const GLvoid*)offsetof(vertex_PCUsg, uv));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUsg), (const GLvoid*)offsetof(vertex_PCUsg, shift));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUsg), (const GLvoid*)offsetof(vertex_PCUsg, gamma));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
	}
	static void DisableVertexAttribArray() BOOST_NOEXCEPT_OR_NOTHROW
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
	}
	inline static const char* default_extension() BOOST_NOEXCEPT_OR_NOTHROW { return "vpcusg"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & shift;
		archive & gamma;
	}
};

// std::ostream& operator << (std::ostream&, const vertex_P&);
// std::ostream& operator << (std::ostream&, const vertex_PC&);
// std::ostream& operator << (std::ostream&, const vertex_PU&);
// std::ostream& operator << (std::ostream&, const vertex_PCU&);
// std::ostream& operator << (std::ostream&, const vertex_PUN&);
// std::ostream& operator << (std::ostream&, const vertex_PCUN&);
// std::ostream& operator << (std::ostream&, const vertex_PUNTB&);
// std::ostream& operator << (std::ostream&, const vertex_PCUNTB&);
// std::ostream& operator << (std::ostream&, const vertex_PCUsg&);

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_P<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PC<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PU<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PCU<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PUN<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PCUN<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PUNTB<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << "], ["
		<< v.tangent[0] << ", " << v.tangent[1] << ", " << v.tangent[2] << "], ["
		<< v.bitangent[0] << ", " << v.bitangent[1] << ", " << v.bitangent[2] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PCUNTB<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << "], ["
		<< v.tangent[0] << ", " << v.tangent[1] << ", " << v.tangent[2] << "], ["
		<< v.bitangent[0] << ", " << v.bitangent[1] << ", " << v.bitangent[2] << ']';
	return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const vertex_PCUsg<T>& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.shift << "], ["
		<< v.gamma << ']';
	return os;
}

/*! \private */
template <typename T>
class has_EnableVertexAttribArray_method
{
	typedef char one;
	typedef long two;

	template <typename C> static one test(decltype(&C::EnableVertexAttribArray));
	template <typename C> static two test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

/*! \endprivate */

} // end of namespace opengl
} // end of namespace gintonic

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#endif