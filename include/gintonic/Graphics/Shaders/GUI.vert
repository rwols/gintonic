
#version 330 core

layout(location = 0) in vec4 iSlot0;

uniform vec2 position;
uniform vec2 scale;

out vec2 texCoords;

void main()
{
	gl_Position = vec4(position + scale * iSlot0.xy, 0.0f, 1.0f);
	texCoords = iSlot0.zw;
}