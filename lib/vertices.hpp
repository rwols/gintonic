#ifndef vertices_hpp
#define vertices_hpp

#include "vec2f.hpp"
#include "vec3f.hpp"
#include "vec4f.hpp"

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#define GINTONIC_VERTEX_LAYOUT_POSITION 0
#define GINTONIC_VERTEX_LAYOUT_TEXCOORD 1
#define GINTONIC_VERTEX_LAYOUT_NORMAL 2
#define GINTONIC_VERTEX_LAYOUT_TANGENT 3
#define GINTONIC_VERTEX_LAYOUT_BITANGENT 4
#define GINTONIC_VERTEX_LAYOUT_VM_MATRIX 5 // 6 7 mat3x4
#define GINTONIC_VERTEX_LAYOUT_N_MATRIX 8 // 9 10 mat3x3
#define GINTONIC_VERTEX_LAYOUT_COLOR 5

#define GT_VERTEX_LAYOUT_POSITION 0
#define GT_VERTEX_LAYOUT_TEXCOORD 1
#define GT_VERTEX_LAYOUT_NORMAL 2
#define GT_VERTEX_LAYOUT_PVM_MATRIX 3 // 4 5 6
#define GT_VERTEX_LAYOUT_VM_MATRIX 7 // 8 9 10
#define GT_VERTEX_LAYOUT_N_MATRIX 11 // 12 13
#define GT_VERTEX_LAYOUT_FREE_14 14
#define GT_VERTEX_LAYOUT_FREE_15 15

namespace GINTONIC_NAMESPACE_FBX
{
	class FbxMesh; // Forward declaration.
	class FbxVector4; // Forward declaration.
	class FbxVector2; // Forward declaration.
	class FbxColor; // Forward declaration.
}

namespace gintonic {
namespace opengl {

struct vertex_P
{
	float position[3]; //!< Position.

	vertex_P() = default;
	vertex_P(const float px, const float py, const float pz);
	vertex_P(const vec3f& p);
	vertex_P(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_P(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_P(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vp"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
	}
	bool operator == (const vertex_P& other) const BOOST_NOEXCEPT_OR_NOTHROW;
	inline bool operator != (const vertex_P& other) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !operator==(other);
	}
};

struct vertex_PC
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.

	vertex_PC() = default;
	vertex_PC(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw);
	vertex_PC(const vec3f& p, const vec4f& c);
	vertex_PC(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PC(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PC(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpc"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
	}
};

struct vertex_PU
{
	float position[3]; //!< Position.
	float uv[2]; //!< Texture UVs.

	vertex_PU() = default;
	vertex_PU(const float px, const float py, const float pz,
		const float ux, const float uy);
	vertex_PU(const vec3f& p, const vec2f& u);
	vertex_PU(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PU(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PU(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpu"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & uv;
	}
};

struct vertex_PN
{
	float position[3]; //!< Position.
	float normal[3]; //!< Normal.
	vertex_PN() = default;
	vertex_PN(const float px, const float py, const float pz,
		const float nx, const float ny, const float nz);
	vertex_PN(const vec3f& p, const vec3f& n);
	vertex_PN(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PN(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".pn"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & normal;
	}
};

struct vertex_PCU
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	vertex_PCU() = default;
	vertex_PCU(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy);
	vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u);
	vertex_PCU(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PCU(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpcu"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
	}
};

struct vertex_PCUN
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.
	vertex_PCUN() = default;
	vertex_PCUN(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy,
		const float nx, const float ny, const float nz);
	vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n);
	vertex_PCUN(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PCUN(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpcun"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & normal;
	}
};

struct vertex_PUN
{
	float position[3]; //!< Position.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.
	vertex_PUN() = default;
	vertex_PUN(const float px, const float py, const float pz,
		const float ux, const float uy,
		const float nx, const float ny, const float nz);
	vertex_PUN(const vec3f& p, const vec2f& u, const vec3f& n);
	vertex_PUN(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PUN(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpun"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & uv;
		archive & normal;
	}
};

struct vertex_PCUNTB
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.
	float tangent[3]; //!< Tangent.
	float bitangent[3]; //!< Bitangent.

	vertex_PCUNTB() {}
	vertex_PCUNTB(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy,
		const float nx, const float ny, const float nz,
		const float tx, const float ty, const float tz,
		const float bx, const float by, const float bz);
	vertex_PCUNTB(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f b);
	vertex_PCUNTB(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PCUNTB(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PCUNTB(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpcuntb"; }
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

struct vertex_PUNTB
{
	float position[3]; //!< Position.
	float uv[2]; //!< Texture UVs.
	float normal[3]; //!< Normal.
	float tangent[3]; //!< Tangent.
	float bitangent[3]; //!< Bitangent.
	vertex_PUNTB() = default;
	vertex_PUNTB(const float px, const float py, const float pz,
		const float ux, const float uy,
		const float nx, const float ny, const float nz,
		const float tx, const float ty, const float tz,
		const float bx, const float by, const float bz);
	vertex_PUNTB(const vec3f& p, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f b);
	vertex_PUNTB(GINTONIC_NAMESPACE_FBX::FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer);
	vertex_PUNTB(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b);
	vertex_PUNTB(const GINTONIC_NAMESPACE_FBX::FbxVector4& p, const GINTONIC_NAMESPACE_FBX::FbxColor& c, const GINTONIC_NAMESPACE_FBX::FbxVector2& u, const GINTONIC_NAMESPACE_FBX::FbxVector4& n, const GINTONIC_NAMESPACE_FBX::FbxVector4& t, const GINTONIC_NAMESPACE_FBX::FbxVector4& b);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpuntb"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & uv;
		archive & normal;
		archive & tangent;
		archive & bitangent;
	}
};

struct vertex_PCUsg
{
	float position[3]; //!< Position.
	float color[4]; //!< Color.
	float uv[2]; //!< Texture UVs.
	float shift; //!< Shift along x.
	float gamma; //!< Color gamma correction.
	vertex_PCUsg() {}
	vertex_PCUsg(const float px, const float py, const float pz,
		const float cx, const float cy, const float cz, const float cw,
		const float ux, const float uy,
		const float shift_value,
		const float gamma_value);
	vertex_PCUsg(const vec3f& p, const vec4f& c, const vec2f& u, const float shift_value, const float gamma_value);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".vpcusg"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & position;
		archive & color;
		archive & uv;
		archive & shift;
		archive & gamma;
	}
};

struct vertex_text2d
{
	float pos_and_texcoord[4];

	vertex_text2d() = default;
	vertex_text2d(const float px, const float py, const float pz, const float pw);
	vertex_text2d(const vec4f& p);
	vertex_text2d(const vec2f& pos, const vec2f& texcoord);
	static void enable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	static void disable_attributes() BOOST_NOEXCEPT_OR_NOTHROW;
	inline static const char* extension() BOOST_NOEXCEPT_OR_NOTHROW { return ".text2d"; }
	template <class Archive> void serialize(Archive& archive, const unsigned int version)
	{
		archive & pos_and_texcoord;
	}
};

std::ostream& operator << (std::ostream&, const vertex_P&);
std::ostream& operator << (std::ostream&, const vertex_PC&);
std::ostream& operator << (std::ostream&, const vertex_PU&);
std::ostream& operator << (std::ostream&, const vertex_PCU&);
std::ostream& operator << (std::ostream&, const vertex_PUN&);
std::ostream& operator << (std::ostream&, const vertex_PCUN&);
std::ostream& operator << (std::ostream&, const vertex_PUNTB&);
std::ostream& operator << (std::ostream&, const vertex_PCUNTB&);
std::ostream& operator << (std::ostream&, const vertex_PCUsg&);
std::ostream& operator << (std::ostream&, const vertex_text2d&);

template <typename T>
class has_enable_attributes_method
{
	typedef char one;
	typedef long two;

	template <typename C> static one test(decltype(&C::enable_attributes));
	template <typename C> static two test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

} // end of namespace opengl
} // end of namespace gintonic

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#endif