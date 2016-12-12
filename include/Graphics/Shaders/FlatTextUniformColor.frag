//
// FlatTextUniformColor.frag
//

#version 330

in vec2 v_texcoord;

uniform vec3 color;
uniform sampler2D glyphTexture;

out vec4 finalColor;

void main()
{
	float alpha = texture(glyphTexture, v_texcoord).r;
	finalColor = vec4(alpha * color, alpha);
}