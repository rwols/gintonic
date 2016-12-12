//
// AmbientLight.frag
//
// Uses LightPass.vert
//

#version 330 core

uniform vec2 viewportSize;

uniform sampler2D geometryBufferDiffuseTexture;

uniform vec4 lightIntensity;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculateScreenUV()
{
	return vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
}

out vec3 final_color;

void main()
{
	vec4 diffuse = texture(geometryBufferDiffuseTexture, calculateScreenUV());
	final_color = (1.0f - diffuse.a) * lightIntensity.a * lightIntensity.rgb * diffuse.rgb;
}