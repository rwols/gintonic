//
// flat_texture.vs
//

#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;


uniform mat4 matrix_PVM;

out vec2 v_uv;

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	v_uv = in_uv;
}