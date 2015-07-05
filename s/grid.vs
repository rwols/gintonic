//
// grid.vs
//

#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 matrix_PVM;

void main()
{
	gl_Position = matrix_PVM * vec4(position, 1.0f);
}