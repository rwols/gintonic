//
// flat_text_uniform_color.fs
//

#version 330 core

in vec2 texcoord;
uniform sampler2D tex;
uniform vec3 color;

out vec4 final_color;

void main()
{
	final_color = vec4(1,1,1,texture2D(tex, texcoord).r) * vec4(color,1);
}