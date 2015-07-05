//
// debug_pass.fs
//

#version 330 core

precision mediump float;

uniform vec3 color;

out vec3 final_color;

void main()
{
	final_color = color;
}