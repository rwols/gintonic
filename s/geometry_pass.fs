//
// geometry_pass.fs
//

#version 330 core

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec4 out_diffuse;
layout (location = 2) out vec3 out_normal;
layout (location = 3) out vec2 out_uv;

uniform struct Material {
	vec4 diffuse_color;
	sampler2D diffuse;
	float diffuse_factor;
} material;

void main()
{
	out_position  = v_position;
	out_diffuse   = material.diffuse_color * texture(material.diffuse, v_texcoord);
	out_diffuse.w = material.diffuse_factor;
	out_normal    = normalize(v_normal);
	out_uv        = v_texcoord;
}
