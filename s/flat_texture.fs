//
// flat_uniform_color.fs
//

#version 330 core

in vec2 v_uv;

uniform struct Material
{
	sampler2D diffuse_color;
	float diffuse_factor;
} material;

out vec4 final_color;

void main()
{
	final_color = texture(material.diffuse_color, v_uv);
}