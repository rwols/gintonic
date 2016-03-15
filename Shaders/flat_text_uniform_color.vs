//
// flat_text_uniform_color.vs
//

#version 330

layout(location = 0) in vec4 incoming;

out vec2 v_texcoord;

void main()
{
	gl_Position = vec4(incoming.xy, 0.0f, 1.0f);
	v_texcoord = incoming.zw;
}