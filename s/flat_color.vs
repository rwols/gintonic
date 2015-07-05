//
// flat_color.vs
//

#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

uniform mat4 matrix_PVM;

out vec4 v_color;

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	v_color = in_color;
}