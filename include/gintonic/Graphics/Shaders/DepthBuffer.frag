
#version 330 core

uniform float farPlane;
uniform vec2 viewportSize;
uniform sampler2D depthTexture;

out vec3 finalColor;

vec2 calculateScreenUV()
{
	return vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
}

void main()
{
	float lDepthValue = texture(depthTexture, calculateScreenUV()).r;
	lDepthValue = (1.0 - lDepthValue) * farPlane * 0.5f;
	finalColor = vec3(lDepthValue);
}