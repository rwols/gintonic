#include "vertices.hpp"
#include "opengl/utilities.hpp"
#include <fbxsdk.h>

namespace gintonic {
namespace opengl {

vertex_P::vertex_P(const float px, const float py, const float pz)
{
	position[0] = px; position[1] = py; position[2] = pz;
}
vertex_P::vertex_P(const vec3f& p)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
}
vertex_P::vertex_P(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0]  = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1]  = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2]  = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
}
vertex_P::vertex_P(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
}
vertex_P::vertex_P(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
}


void vertex_P::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_P), (const GLvoid*)offsetof(vertex_P, position));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
}
void vertex_P::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
}
bool vertex_P::operator == (const vertex_P& other) const noexcept
{
	return position[0] == other.position[0] && position[1] == other.position[1]
		&& position[2] == other.position[2];
}

vertex_PC::vertex_PC(const float px, const float py, const float pz,
	const float cx, const float cy, const float cz, const float cw)
{
	position[0] = px; position[1] = py; position[2] = pz;
	color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
}
vertex_PC::vertex_PC(const vec3f& p, const vec4f& c)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	color[0] = c.x; color[1] = c.y; color[2] = c.z; color[3] = c.w;
}
vertex_PC::vertex_PC(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0] = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1] = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2] = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	auto fbx_color_layer = pMesh->GetLayer(layer)->GetVertexColors();
	auto fbx_color = fbx_color_layer->GetDirectArray()[fbx_color_layer->GetIndexArray()[i]];
	color[0] = fbx_color[0];
	color[1] = fbx_color[1];
	color[2] = fbx_color[2];
	color[3] = fbx_color[3];
}
vertex_PC::vertex_PC(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	color[0] = static_cast<float>(c.x);
	color[1] = static_cast<float>(c.y);
	color[2] = static_cast<float>(c.z);
	color[3] = static_cast<float>(c.w);
}
vertex_PC::vertex_PC(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = p[0];
	position[1] = p[1];
	position[2] = p[2];
	color[0] = c[0];
	color[1] = c[1];
	color[2] = c[2];
	color[3] = c[3];
}


void vertex_PC::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PC), (const GLvoid*)offsetof(vertex_PC, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PC), (const GLvoid*)offsetof(vertex_PC, color));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
}
void vertex_PC::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
}


vertex_PU::vertex_PU(const float px, const float py, const float pz,
	const float ux, const float uy)
{
	position[0] = px; position[1] = py; position[2] = pz;
	uv[0] = ux; uv[1] = uy;
}
vertex_PU::vertex_PU(const vec3f& p, const vec2f& u)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	uv[0] = u.x; uv[1] = u.y;
}
vertex_PU::vertex_PU(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0] = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1] = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2] = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	uv[0]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
	uv[1]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
}
vertex_PU::vertex_PU(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	uv[0] = static_cast<float>(u.x);
	uv[1] = static_cast<float>(u.y);
}
vertex_PU::vertex_PU(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	uv[0] = static_cast<float>(u[0]);
	uv[1] = static_cast<float>(u[1]);
}

void vertex_PU::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PU), (const GLvoid*)offsetof(vertex_PU, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PU), (const GLvoid*)offsetof(vertex_PU, uv));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
}
void vertex_PU::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
}

vertex_PN::vertex_PN(const float px, const float py, const float pz,
	const float nx, const float ny, const float nz)
{
	position[0] = px; position[1] = py; position[2] = pz;
	normal[0] = nx; normal[1] = ny; normal[2] = nz;
}
vertex_PN::vertex_PN(const vec3f& p, const vec3f& n)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	normal[0] = n.x; normal[1] = n.y; normal[2] = n.z;
}
vertex_PN::vertex_PN(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0]  = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1]  = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2]  = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	normal[0]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
	normal[1]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
	normal[2]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
}
vertex_PN::vertex_PN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	normal[0] = static_cast<float>(n.x);
	normal[1] = static_cast<float>(n.y);
	normal[2] = static_cast<float>(n.z);
}
vertex_PN::vertex_PN(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	normal[0] = static_cast<float>(n[0]);
	normal[1] = static_cast<float>(n[1]);
	normal[2] = static_cast<float>(n[2]);
}
void vertex_PN::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PN), (const GLvoid*)offsetof(vertex_PN, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PN), (const GLvoid*)offsetof(vertex_PN, normal));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
}
void vertex_PN::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
}

vertex_PCU::vertex_PCU(const float px, const float py, const float pz,
	const float cx, const float cy, const float cz, const float cw,
	const float ux, const float uy)
{
	position[0] = px; position[1] = py; position[2] = pz;
	color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
	uv[0] = ux; uv[1] = uy;
}
vertex_PCU::vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	color[0] = c.x; color[1] = c.y; color[2] = c.z; color[3] = c.w;
	uv[0] = u.x; uv[1] = u.y;
}
vertex_PCU::vertex_PCU(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0] = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1] = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2] = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	color[0]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
	color[1]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
	color[2]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
	color[3]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
	uv[0]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
	uv[1]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
}
vertex_PCU::vertex_PCU(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	color[0] = static_cast<float>(c.x);
	color[1] = static_cast<float>(c.y);
	color[2] = static_cast<float>(c.z);
	color[3] = static_cast<float>(c.w);
	uv[0] = static_cast<float>(u.x);
	uv[1] = static_cast<float>(u.y);
}
vertex_PCU::vertex_PCU(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	color[0] = static_cast<float>(c[0]);
	color[1] = static_cast<float>(c[1]);
	color[2] = static_cast<float>(c[2]);
	color[3] = static_cast<float>(c[3]);
	uv[0] = static_cast<float>(u[0]);
	uv[1] = static_cast<float>(u[1]);
}

void vertex_PCU::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCU), (const GLvoid*)offsetof(vertex_PCU, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCU), (const GLvoid*)offsetof(vertex_PCU, color));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCU), (const GLvoid*)offsetof(vertex_PCU, uv));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
}
void vertex_PCU::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
}

vertex_PCUN::vertex_PCUN(const float px, const float py, const float pz,
	const float cx, const float cy, const float cz, const float cw,
	const float ux, const float uy,
	const float nx, const float ny, const float nz)
{
	position[0] = px; position[1] = py; position[2] = pz;
	color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
	uv[0] = ux; uv[1] = uy;
	normal[0] = nx; normal[1] = ny; normal[2] = nz;
}
vertex_PCUN::vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	color[0] = c.x; color[1] = c.y; color[2] = c.z; color[3] = c.w;
	uv[0] = u.x; uv[1] = u.y;
	normal[0] = n.x; normal[1] = n.y; normal[2] = n.z;
}
vertex_PCUN::vertex_PCUN(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0] = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1] = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2] = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	color[0]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
	color[1]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
	color[2]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
	color[3]    = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
	uv[0]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
	uv[1]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
	normal[0]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
	normal[1]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
	normal[2]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
}
vertex_PCUN::vertex_PCUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	color[0] = static_cast<float>(c.x);
	color[1] = static_cast<float>(c.y);
	color[2] = static_cast<float>(c.z);
	color[3] = static_cast<float>(c.w);
	uv[0] = static_cast<float>(u.x);
	uv[1] = static_cast<float>(u.y);
	normal[0] = static_cast<float>(n.x);
	normal[1] = static_cast<float>(n.y);
	normal[2] = static_cast<float>(n.z);
}
vertex_PCUN::vertex_PCUN(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	color[0] = static_cast<float>(c[0]);
	color[1] = static_cast<float>(c[1]);
	color[2] = static_cast<float>(c[2]);
	color[3] = static_cast<float>(c[3]);
	uv[0] = static_cast<float>(u[0]);
	uv[1] = static_cast<float>(u[1]);
	normal[0] = static_cast<float>(n[0]);
	normal[1] = static_cast<float>(n[1]);
	normal[2] = static_cast<float>(n[2]);
}
void vertex_PCUN::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, color));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, uv));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUN), (const GLvoid*)offsetof(vertex_PCUN, normal));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
}
void vertex_PCUN::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
}

vertex_PUN::vertex_PUN(const float px, const float py, const float pz,
	const float ux, const float uy,
	const float nx, const float ny, const float nz)
{
	position[0] = px; position[1] = py; position[2] = pz;
	uv[0] = ux; uv[1] = uy;
	normal[0] = nx; normal[1] = ny; normal[2] = nz;
}
vertex_PUN::vertex_PUN(const vec3f& p, const vec2f& u, const vec3f& n)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	uv[0] = u.x; uv[1] = u.y;
	normal[0] = n.x; normal[1] = n.y; normal[2] = n.z;
}
vertex_PUN::vertex_PUN(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0]  = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1]  = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2]  = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	uv[0]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
	uv[1]       = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
	normal[0]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
	normal[1]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
	normal[2]   = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
}
vertex_PUN::vertex_PUN(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	uv[0] = static_cast<float>(u.x);
	uv[1] = static_cast<float>(u.y);
	normal[0] = static_cast<float>(n.x);
	normal[1] = static_cast<float>(n.y);
	normal[2] = static_cast<float>(n.z);
}
vertex_PUN::vertex_PUN(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	uv[0] = static_cast<float>(u[0]);
	uv[1] = static_cast<float>(u[1]);
	normal[0] = static_cast<float>(n[0]);
	normal[1] = static_cast<float>(n[1]);
	normal[2] = static_cast<float>(n[2]);
}
void vertex_PUN::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUN), (const GLvoid*)offsetof(vertex_PUN, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PUN), (const GLvoid*)offsetof(vertex_PUN, uv));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUN), (const GLvoid*)offsetof(vertex_PUN, normal));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
}
void vertex_PUN::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
}

vertex_PCUNTB::vertex_PCUNTB(const float px, const float py, const float pz,
	const float cx, const float cy, const float cz, const float cw,
	const float ux, const float uy,
	const float nx, const float ny, const float nz,
	const float tx, const float ty, const float tz,
	const float bx, const float by, const float bz)
{
	position[0] = px; position[1] = py; position[2] = pz;
	color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
	uv[0] = ux; uv[1] = uy;
	normal[0] = nx; normal[1] = ny; normal[2] = nz;
	tangent[0] = tx; tangent[1] = ty; tangent[2] = tz;
	bitangent[0] = bx; bitangent[1] = by; tangent[2] = bz;
}
vertex_PCUNTB::vertex_PCUNTB(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0]  = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1]  = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2]  = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	color[0]     = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][0]);
	color[1]     = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][1]);
	color[2]     = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][2]);
	color[3]     = static_cast<float>(pMesh->GetLayer(0)->GetVertexColors()->GetDirectArray()[i][3]);
	uv[0]        = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
	uv[1]        = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
	normal[0]    = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
	normal[1]    = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
	normal[2]    = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
	tangent[0]   = static_cast<float>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][0]);
	tangent[1]   = static_cast<float>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][1]);
	tangent[2]   = static_cast<float>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][2]);
	bitangent[0] = static_cast<float>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][0]);
	bitangent[1] = static_cast<float>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][1]);
	bitangent[2] = static_cast<float>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][2]);
}
vertex_PCUNTB::vertex_PCUNTB(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	color[0] = static_cast<float>(c.x);
	color[1] = static_cast<float>(c.y);
	color[2] = static_cast<float>(c.z);
	color[3] = static_cast<float>(c.w);
	uv[0] = static_cast<float>(u.x);
	uv[1] = static_cast<float>(u.y);
	normal[0] = static_cast<float>(n.x);
	normal[1] = static_cast<float>(n.y);
	normal[2] = static_cast<float>(n.z);
	tangent[0] = static_cast<float>(t.x);
	tangent[1] = static_cast<float>(t.y);
	tangent[2] = static_cast<float>(t.z);
	bitangent[0] = static_cast<float>(b.x);
	bitangent[1] = static_cast<float>(b.y);
	bitangent[2] = static_cast<float>(b.z);
}
vertex_PCUNTB::vertex_PCUNTB(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	color[0] = static_cast<float>(c[0]);
	color[1] = static_cast<float>(c[1]);
	color[2] = static_cast<float>(c[2]);
	color[3] = static_cast<float>(c[3]);
	uv[0] = static_cast<float>(u[0]);
	uv[1] = static_cast<float>(u[1]);
	normal[0] = static_cast<float>(n[0]);
	normal[1] = static_cast<float>(n[1]);
	normal[2] = static_cast<float>(n[2]);
	tangent[0] = static_cast<float>(t[0]);
	tangent[1] = static_cast<float>(t[1]);
	tangent[2] = static_cast<float>(t[2]);
	bitangent[0] = static_cast<float>(b[0]);
	bitangent[1] = static_cast<float>(b[1]);
	bitangent[2] = static_cast<float>(b[2]);
}
void vertex_PCUNTB::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, color));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, uv));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, normal));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, tangent));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PCUNTB), (const GLvoid*)offsetof(vertex_PCUNTB, bitangent));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TANGENT);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_BITANGENT);
}
void vertex_PCUNTB::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_COLOR);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TANGENT);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_BITANGENT);
}

vertex_PUNTB::vertex_PUNTB(const float px, const float py, const float pz,
	const float ux, const float uy,
	const float nx, const float ny, const float nz,
	const float tx, const float ty, const float tz,
	const float bx, const float by, const float bz)
{
	position[0] = px; position[1] = py; position[2] = pz;
	uv[0] = ux; uv[1] = uy;
	normal[0] = nx; normal[1] = ny; normal[2] = nz;
	tangent[0] = tx; tangent[1] = ty; tangent[2] = tz;
	bitangent[0] = bx; bitangent[1] = by; tangent[2] = bz;
}
vertex_PUNTB::vertex_PUNTB(const vec3f& p, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	uv[0] = u.x; uv[1] = u.y;
	normal[0] = n.x; normal[1] = n.y; normal[2] = n.z;
	tangent[0] = t.x; tangent[1] = t.y; tangent[2] = t.z;
	bitangent[0] = b.x; bitangent[1] = b.y; bitangent[2] = b.z;
}
vertex_PUNTB::vertex_PUNTB(FbxMesh const*const pMesh, const std::size_t i, const std::size_t layer)
{
	position[0]  = static_cast<float>(pMesh->GetControlPointAt(i)[0]);
	position[1]  = static_cast<float>(pMesh->GetControlPointAt(i)[1]);
	position[2]  = static_cast<float>(pMesh->GetControlPointAt(i)[2]);
	uv[0]        = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][0]);
	uv[1]        = static_cast<float>(pMesh->GetLayer(0)->GetUVs()->GetDirectArray()[i][1]);
	normal[0]    = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][0]);
	normal[1]    = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][1]);
	normal[2]    = static_cast<float>(pMesh->GetLayer(0)->GetNormals()->GetDirectArray()[i][2]);
	tangent[0]   = static_cast<float>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][0]);
	tangent[1]   = static_cast<float>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][1]);
	tangent[2]   = static_cast<float>(pMesh->GetLayer(0)->GetTangents()->GetDirectArray()[i][2]);
	bitangent[0] = static_cast<float>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][0]);
	bitangent[1] = static_cast<float>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][1]);
	bitangent[2] = static_cast<float>(pMesh->GetLayer(0)->GetBinormals()->GetDirectArray()[i][2]);
}
vertex_PUNTB::vertex_PUNTB(const vec3f& p, const vec4f& c, const vec2f& u, const vec3f& n, const vec3f& t, const vec3f& b)
{
	position[0] = static_cast<float>(p.x);
	position[1] = static_cast<float>(p.y);
	position[2] = static_cast<float>(p.z);
	uv[0] = static_cast<float>(u.x);
	uv[1] = static_cast<float>(u.y);
	normal[0] = static_cast<float>(n.x);
	normal[1] = static_cast<float>(n.y);
	normal[2] = static_cast<float>(n.z);
	tangent[0] = static_cast<float>(t.x);
	tangent[1] = static_cast<float>(t.y);
	tangent[2] = static_cast<float>(t.z);
	bitangent[0] = static_cast<float>(b.x);
	bitangent[1] = static_cast<float>(b.y);
	bitangent[2] = static_cast<float>(b.z);
}
vertex_PUNTB::vertex_PUNTB(const FbxVector4& p, const FbxColor& c, const FbxVector2& u, const FbxVector4& n, const FbxVector4& t, const FbxVector4& b)
{
	position[0] = static_cast<float>(p[0]);
	position[1] = static_cast<float>(p[1]);
	position[2] = static_cast<float>(p[2]);
	uv[0] = static_cast<float>(u[0]);
	uv[1] = static_cast<float>(u[1]);
	normal[0] = static_cast<float>(n[0]);
	normal[1] = static_cast<float>(n[1]);
	normal[2] = static_cast<float>(n[2]);
	tangent[0] = static_cast<float>(t[0]);
	tangent[1] = static_cast<float>(t[1]);
	tangent[2] = static_cast<float>(t[2]);
	bitangent[0] = static_cast<float>(b[0]);
	bitangent[1] = static_cast<float>(b[1]);
	bitangent[2] = static_cast<float>(b[2]);
}
void vertex_PUNTB::enable_attributes() noexcept
{
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, position));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, uv));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, normal));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, tangent));
	glVertexAttribPointer(GINTONIC_VERTEX_LAYOUT_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_PUNTB), (const GLvoid*)offsetof(vertex_PUNTB, bitangent));
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TANGENT);
	glEnableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_BITANGENT);
}
void vertex_PUNTB::disable_attributes() noexcept
{
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_POSITION);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TEXCOORD);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_NORMAL);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_TANGENT);
	glDisableVertexAttribArray(GINTONIC_VERTEX_LAYOUT_BITANGENT);
}

vertex_PCUsg::vertex_PCUsg(const float px, const float py, const float pz,
	const float cx, const float cy, const float cz, const float cw,
	const float ux, const float uy,
	const float shift_value,
	const float gamma_value)
{
	position[0] = px; position[1] = py; position[2] = pz;
	color[0] = cx; color[1] = cy; color[2] = cz; color[3] = cw;
	uv[0] = ux; uv[1] = uy;
	shift = shift_value;
	gamma = gamma_value;
}
vertex_PCUsg::vertex_PCUsg(const vec3f& p, const vec4f& c, const vec2f& u, const float shift_value, const float gamma_value)
{
	position[0] = p.x; position[1] = p.y; position[2] = p.z;
	color[0] = c.x; color[1] = c.y; color[2] = c.z; color[3] = c.w;
	uv[0] = u.x; uv[1] = u.y;
	shift = shift_value;
	gamma = gamma_value;
}
void vertex_PCUsg::enable_attributes() noexcept
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
void vertex_PCUsg::disable_attributes() noexcept
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

vertex_text2d::vertex_text2d(const float px, const float py, const float pz, const float pw)
{
	pos_and_texcoord[0] = px;
	pos_and_texcoord[1] = py;
	pos_and_texcoord[2] = pz;
	pos_and_texcoord[3] = pw;
}
vertex_text2d::vertex_text2d(const vec4f& p)
{
	pos_and_texcoord[0] = p.x;
	pos_and_texcoord[1] = p.y;
	pos_and_texcoord[2] = p.z;
	pos_and_texcoord[3] = p.w;
}
vertex_text2d::vertex_text2d(const vec2f& pos, const vec2f& texcoord)
{
	pos_and_texcoord[0] = pos.x;
	pos_and_texcoord[1] = pos.y;
	pos_and_texcoord[2] = texcoord.x;
	pos_and_texcoord[3] = texcoord.y;
}
void vertex_text2d::enable_attributes() noexcept
{
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_text2d), (const GLvoid*)offsetof(vertex_text2d, pos_and_texcoord));
	glEnableVertexAttribArray(0);

}
void vertex_text2d::disable_attributes() noexcept
{
	glDisableVertexAttribArray(0);
}

std::ostream& operator << (std::ostream& os, const vertex_P& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PC& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PU& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PCU& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PUN& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PCUN& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PUNTB& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << "], ["
		<< v.tangent[0] << ", " << v.tangent[1] << ", " << v.tangent[2] << "], ["
		<< v.bitangent[0] << ", " << v.bitangent[1] << ", " << v.bitangent[2] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PCUNTB& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2] << "], ["
		<< v.tangent[0] << ", " << v.tangent[1] << ", " << v.tangent[2] << "], ["
		<< v.bitangent[0] << ", " << v.bitangent[1] << ", " << v.bitangent[2] << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_PCUsg& v)
{
	os << '[' << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << "], ["
		<< v.color[0] << ", " << v.color[1] << ", " << v.color[2] << ", " << v.color[3] << "], ["
		<< v.uv[0] << ", " << v.uv[1] << "], ["
		<< v.shift << "], ["
		<< v.gamma << ']';
	return os;
}

std::ostream& operator << (std::ostream& os, const vertex_text2d& v)
{
	os << '[' << v.pos_and_texcoord[0] << ", " << v.pos_and_texcoord[1]
		<< ", " << v.pos_and_texcoord[2] << ", " << v.pos_and_texcoord[3]
		<< ']';
	return os;
}

} // end of namespace opengl
} // end of namespace gintonic