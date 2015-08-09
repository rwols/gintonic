//
// gp_dn.fs
//
// Uses gp_n.vs as vertex shader.

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_COUNT 4

in vec3 v_position; // Incoming VIEW space position
in vec2 v_texcoord; // Incoming TEXTURE space texcoord
in mat3 v_matrix_T; // TANGENT->VIEW matrix

layout (location = GBUFFER_POSITION) out vec3 out_position; // VIEW space
layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse; // TEXTURE space
layout (location = GBUFFER_SPECULAR) out vec4 out_specular; // TEXTURE space
layout (location = GBUFFER_NORMAL) out vec3 out_normal; // VIEW space

uniform struct Material 
{
	vec4 diffuse_color;
	vec4 specular_color;
	sampler2D specular_texture;
	sampler2D normal_texture;
} material;

void main()
{
	out_position = v_position;
	out_diffuse = material.diffuse_color;
	out_specular = material.specular_color;
	out_specular *= texture(material.specular_texture, v_texcoord);

	// Sample the normal vector from the materials' "normal texture".
	vec3 normal = texture(material.normal_texture, v_texcoord).rgb;

	// The normal vector is encoded in RGB colors, so we need
	// to decode that.
	normal = 2.0f * normal - 1.0f;

	// This normal vector is in TANGENT space. So
	// an unperturbed normal has coordinates (0,1,0).
	// The geometry buffer expects normals to be in VIEW space.
	// So we need to transform from TANGENT space to VIEW space.
	out_normal = normalize(v_matrix_T * normal);
}
