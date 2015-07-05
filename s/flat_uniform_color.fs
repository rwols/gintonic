//
// flat_uniform_color.fs
//

#version 330 core

uniform struct Material
{
	sampler2D diffuse_color;
	float diffuse_factor;
} material;

out vec4 final_color;

void main()
{
	final_color = material.diffuse_color;
}