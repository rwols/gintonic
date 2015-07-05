//
// Tutorial3.vs
//

#version 330 core

uniform mat4 ProjectionViewModelMatrix;

layout(location = 0) in vec3 Position;

void main()
{
	gl_Position = ProjectionViewModelMatrix * vec4(Position, 1.0f);
}