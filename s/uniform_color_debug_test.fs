//
// uniform_color_debug_test.fs
//

#version 330 core

uniform vec4 color;
out vec4 final_color;

void main()
{
	final_color = color;
}