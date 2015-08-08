//
// gp_cd.fs
//
// Geometry Pass: Color and Diffuse
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_COUNT 4

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

layout (location = GBUFFER_POSITION) out vec3 out_position;
layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse;
layout (location = GBUFFER_SPECULAR) out vec4 out_specular;
layout (location = GBUFFER_NORMAL) out vec3 out_normal;

uniform struct Material {
	vec4 color;
	sampler2D diffuse;
} material;

void main()
{
	out_position = v_position;
	out_diffuse  = material.color * texture(material.diffuse, v_texcoord);
	out_specular = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	out_normal   = normalize(v_normal);
}
