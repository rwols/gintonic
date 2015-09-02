#include "mesh.hpp"
#include "entity.hpp"
#include <fbxsdk.h>

namespace gintonic {

void mesh::attach(entity& e)
{
	if (e.m_mesh_component == this) return;
	else if (e.m_mesh_component) e.m_mesh_component->detach(e);
	e.m_mesh_component = this;
	m_ents.push_back(&e);
}

void mesh::detach(entity& e)
{
	for (auto i = begin(); i != end(); ++i)
	{
		if (*i == &e)
		{
			e.m_mesh_component = nullptr;
			m_ents.erase(i);
			return;
		}
	}
}

mesh::vec2f::vec2f(const FbxVector2& v)
: x(static_cast<GLfloat>(v[0]))
, y(static_cast<GLfloat>(v[1]))
{
	/* Empty on purpose. */
}

mesh::vec2f::vec2f(const GLfloat x, const GLfloat y)
: x(x)
, y(y)
{
	/* Empty on purpose. */
}

bool mesh::vec2f::operator==(const mesh::vec2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return x == other.x && y == other.y;
}

bool mesh::vec2f::operator!=(const mesh::vec2f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return !operator==(other);
}

void mesh::vec2f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(mesh::vec2f), nullptr);
}

mesh::vec3f::vec3f(const FbxVector4& v)
: x(static_cast<GLfloat>(v[0]))
, y(static_cast<GLfloat>(v[1]))
, z(static_cast<GLfloat>(v[2]))
{
	/* Empty on purpose. */
}

mesh::vec3f::vec3f(const GLfloat x, const GLfloat y, const GLfloat z)
: x(x)
, y(y)
, z(z)
{
	/* Empty on purpose. */
}

bool mesh::vec3f::operator==(const mesh::vec3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return x == other.x && y == other.y && z == other.z;
}

bool mesh::vec3f::operator!=(const mesh::vec3f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return !operator==(other);
}

void mesh::vec3f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(mesh::vec3f), nullptr);
}

mesh::vec4f::vec4f(const FbxVector4& v)
: x(static_cast<GLfloat>(v[0]))
, y(static_cast<GLfloat>(v[1]))
, z(static_cast<GLfloat>(v[2]))
, w(static_cast<GLfloat>(v[3]))
{
	/* Empty on purpose. */
}

mesh::vec4f::vec4f(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
: x(x)
, y(y)
, z(z)
, w(w)
{
	/* Empty on purpose. */
}

mesh::~mesh() BOOST_NOEXCEPT_OR_NOTHROW
{
	for (auto* e : m_ents) e->m_mesh_component = nullptr;
}

bool mesh::vec4f::operator==(const mesh::vec4f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool mesh::vec4f::operator!=(const mesh::vec4f& other) const BOOST_NOEXCEPT_OR_NOTHROW
{
	return !operator==(other);
}

void mesh::vec4f::enable_attribute(const GLuint index) BOOST_NOEXCEPT_OR_NOTHROW
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(mesh::vec4f), nullptr);
}

void mesh::draw(
	const std::vector<mat4f, allocator<mat4f>>& PVM_matrices,
	const std::vector<mat4f, allocator<mat4f>>& VM_matrices,
	const std::vector<mat3f, allocator<mat3f>>& N_matrices)
{
	// Do nothing.
	return;
}

} // namespace gintonic