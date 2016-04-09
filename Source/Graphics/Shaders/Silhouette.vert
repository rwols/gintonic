
#version 330 core

layout(location = 0) in vec3 iPosition;

uniform mat4 matrixPVM;
uniform mat4 matrixVM;

out vec3 viewSpaceVertexPosition;

void main()
{
	gl_Position = matrixPVM * vec4(iPosition, 1.0f);
	viewSpaceVertexPosition = (matrixVM * vec4(iPosition, 1.0f)).xyz;
}