
#version 330 core

in vec2 texCoords;

uniform vec4 color4;
uniform int hasGUITexture;
uniform int withoutColorAlpha;
uniform sampler2D GUITexture;

out vec4 finalColor;

void main()
{
	vec4 texColor = (hasGUITexture != 0) ? texture(GUITexture, texCoords) : vec4(1.0f, 1.0f, 1.0f, 1.0f);
	finalColor = (withoutColorAlpha == 0) ? vec4(color4.xyz * texColor.r, texColor.r) : color4 * texColor;
}