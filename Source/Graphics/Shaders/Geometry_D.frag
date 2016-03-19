//
// Geometry_D.frag
//
// Uses Geometry.vert as vertex shader

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_COUNT 4

in vec3 viewPosition;
in vec2 texCoords;
in vec3 viewNormal;

layout (location = GBUFFER_POSITION) out vec3 outPosition;
layout (location = GBUFFER_DIFFUSE) out vec4 outDiffuse;
layout (location = GBUFFER_SPECULAR) out vec4 outSpecular;
layout (location = GBUFFER_NORMAL) out vec3 outNormal;

uniform struct Material 
{
	vec4 diffuse_color;
	vec4 specular_color;
	sampler2D diffuse_texture;
} material;

void main()
{
	outPosition = viewPosition;
	outDiffuse = material.diffuse_color;
	outDiffuse *= texture(material.diffuse_texture, texCoords);
	outSpecular = material.specular_color;
	outNormal = normalize(viewNormal);
}
