
#version 330 core

layout(location = 0) in vec3 iPosition;

// uniform mat4 matrixVM;

void main()
{
	gl_Position = vec4(iPosition, 1.0f);
	// gl_Position = matrixVM * vec4(iPosition, 1.0f);
}