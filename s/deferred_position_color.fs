//
// deferred_position_color.fs
//

#version 330 core

in vec3 view_position;
in vec4 color;

layout(location = 0) out vec3 positions;
layout(location = 1) out vec4 colors;

void main()
{
	positions = view_position;
	colors = color;
}