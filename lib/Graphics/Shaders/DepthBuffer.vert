
#version 330 core

layout (location = 0) in vec4 iSlot0;

void main()
{
	gl_Position = vec4(iSlot0.xyz, 1.0f);
}