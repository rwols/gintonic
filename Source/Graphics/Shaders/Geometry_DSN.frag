//
// Geometry_DSN.frag
//
// Uses Geometry_N.vert as vertex shader.

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_COUNT 4

in vec3 viewPosition; // Incoming VIEW space position
in vec2 texCoords; // Incoming TEXTURE space texcoord
in mat3 tangentMatrix; // Incoming TANGENT->VIEW matrix

layout (location = GBUFFER_POSITION) out vec3 outPosition; // VIEW space
layout (location = GBUFFER_DIFFUSE) out vec4 outDiffuse; // TEXTURE space
layout (location = GBUFFER_SPECULAR) out vec4 outSpecular; // TEXTURE space
layout (location = GBUFFER_NORMAL) out vec3 outNormal; // VIEW space

uniform struct Material 
{
	vec4 diffuse_color;
	vec4 specular_color;
	sampler2D diffuse_texture;
	sampler2D specular_texture;
	sampler2D normal_texture;
} material;

void main()
{
	outPosition = viewPosition;
	outDiffuse = material.diffuse_color;
	outDiffuse *= texture(material.diffuse_texture, texCoords);
	outSpecular = material.specular_color;
	outSpecular *= texture(material.specular_texture, texCoords);

	// Sample the normal vector from the material's "normal texture".
	vec3 normal = texture(material.normal_texture, texCoords).rgb;

	// The normal vector is encoded in RGB colors, so we need
	// to decode that.
	normal = 2.0f * normal - 1.0f;

	// This normal vector is in TANGENT space. So
	// an unperturbed normal has coordinates (0,0,1).
	// The geometry buffer expects normals to be in VIEW space.
	// So we need to transform from TANGENT space to VIEW space.
	outNormal = normalize(tangentMatrix * normal);
}