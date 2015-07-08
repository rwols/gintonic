//
// pn.vs
//
// forward lighting with incoming position and normal
//

#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 v_position;
out vec3 v_normal;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1);
	v_position = matrix_VM * vec4(in_position, 1);
	v_normal = matrix_N * in_normal;
}