//
// phong_brdf
//

#version 330 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec3 i_normal;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 v_normal;
out vec3 v_position;

void main()
{
	gl_Position = matrix_PVM * vec4(i_position, 1.0f);
	v_position  = (matrix_VM * vec4(i_position, 1.0f)).xyz;
	v_normal    = vec3(matrix_N * vec4(i_normal, 0.0f));
}