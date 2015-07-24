//
// pun_point.vs
//
// forward lighting with incoming position, UV and normal
//

#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 0) in vec2 in_texcoord;
layout (location = 1) in vec3 in_normal;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	v_position = (matrix_VM * vec4(in_position, 1.0f)).xyz;
	v_texcoord = in_texcoord;
	v_normal = matrix_N * in_normal;
}