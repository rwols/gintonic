//
// Tutorial4.vs
//

#version 330 core

uniform mat4 ProjectionViewModelMatrix;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Uv;

out vec2 vUv;

void main()
{
	gl_Position = ProjectionViewModelMatrix * vec4(Position, 1.0f);
	vUv = Uv;
}