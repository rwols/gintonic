//
// gp_dt.fs
//
// Geometry Pass: Diffuse Texture
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_TEXCOORD 4
#define GBUFFER_COUNT 5

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

layout (location = GBUFFER_POSITION) out vec3 out_position;
layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse;
layout (location = GBUFFER_NORMAL) out vec3 out_normal;

uniform struct Material {
	sampler2D diffuse_texture;
} material;

void main()
{
	out_position  = v_position;
	out_diffuse   = texture(material.diffuse_texture, v_texcoord);
	out_normal    = normalize(v_normal);
}
