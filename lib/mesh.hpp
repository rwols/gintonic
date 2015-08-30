#ifndef gintonic_mesh_hpp
#define gintonic_mesh_hpp

#include "opengl/glad.hpp"
#include "component.hpp"
#include "mat3f.hpp"
#include "mat4f.hpp"
#include "allocator.hpp"
#include <vector>

namespace FBX
{
	class FbxVector2; // Forward declaration.
	class FbxVector4; // Forward declaration.
} // namespace FBX

namespace gintonic {

class mesh : public component
{
public:

	enum mesh_type
	{
		kStaticMesh,
		kDynamicMesh
	};

	virtual void attach(entity&);
	virtual void detach(entity&);

	virtual void draw() const BOOST_NOEXCEPT_OR_NOTHROW = 0;

	virtual void draw(
		const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
		const std::vector<mat4f, allocator<mat4f>>& VM_matrices,
		const std::vector<mat3f, allocator<mat3f>>& N_matrices);

	inline mesh_type type() const BOOST_NOEXCEPT_OR_NOTHROW { return m_type; }

	virtual ~mesh() BOOST_NOEXCEPT_OR_NOTHROW;

	struct vec2f
	{
		GLfloat x;
		GLfloat y;

		vec2f() = default;
		vec2f(const FBX::FbxVector2&);
		vec2f(const GLfloat x, const GLfloat y);

		bool operator == (const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;
		bool operator != (const vec2f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
	};

	struct vec3f
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;

		vec3f() = default;
		vec3f(const FBX::FbxVector4&);
		vec3f(const GLfloat x, const GLfloat y, const GLfloat z);

		bool operator == (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;
		bool operator != (const vec3f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
	};

	struct vec4f
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat w;

		vec4f() = default;
		vec4f(const FBX::FbxVector4&);
		vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);

		bool operator == (const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;
		bool operator != (const vec4f&) const BOOST_NOEXCEPT_OR_NOTHROW;

		static void enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW;
	};

protected:

	inline mesh(const mesh_type t) : m_type(t) {}

private:

	mesh_type m_type;

};

} // namespace gintonic

#endif