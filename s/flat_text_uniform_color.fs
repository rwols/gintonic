//
// flat_text_uniform_color.fs
//

#version 330 core

in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;

out vec4 final_color;

void main()
{
	final_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}