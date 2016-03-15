//
// flat_text_uniform_color.fs
//

#version 330

in vec2 v_texcoord;

uniform vec3 color;
uniform sampler2D tex;

out vec4 final_color;

void main()
{
	float alpha = texture(tex, v_texcoord).r;
	final_color = vec4(alpha * color, alpha);
}