//
// flat_uniform_color.vs
//

#version 330 core

layout(location = 0) in vec3 in_position;

uniform mat4 matrix_PVM;

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
}