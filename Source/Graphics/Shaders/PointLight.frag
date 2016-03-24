//
// PointLight.frag
//
// Uses LightPassPVM.vert
//

#version 330 core

// #define NDEBUG

uniform vec2 viewportSize;

#ifndef NDEBUG
uniform int debugFlag;
#endif

uniform sampler2D geometryBufferPositionTexture;
uniform sampler2D geometryBufferDiffuseTexture;
uniform sampler2D geometryBufferSpecularTexture;
uniform sampler2D geometryBufferNormalTexture;

uniform vec4 lightIntensity;
uniform vec3 lightPosition;
uniform vec4 lightAttenuation;

out vec3 finalColor;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculateScreenPosition()
{
	return vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
}

float maxdot(in vec3 A, in vec3 B)
{
	return max(dot(A,B), 0.0f);
}

float clamppowmaxdot(in vec3 A, in vec3 B, in float exponent)
{
	return clamp(pow(maxdot(A,B), exponent), 0.0f, 1.0f);
}

float quadraticPoly(in float a, in float b, in float c, in float x)
{
	return a + b * x + c * x * x;
}

void main()
{
	vec2 lScreenUV = calculateScreenPosition();

	vec3 lViewSpaceVertexPosition = texture(geometryBufferPositionTexture, lScreenUV).xyz;
	vec4 lDiffuseColor            = texture(geometryBufferDiffuseTexture,  lScreenUV);
	vec4 lSpecularColor           = texture(geometryBufferSpecularTexture, lScreenUV);
	vec3 lViewSpaceVertexNormal   = texture(geometryBufferNormalTexture,   lScreenUV).xyz;

	// L is the direction from the surface position to the light position
	// P is in VM-coordinates, so lightPosition must be supplied in
	// VM-coordinates too.
	vec3 L = lightPosition - lViewSpaceVertexPosition;

	// d is the distance from the surface position to the light position
	float d = length(L);

	float dc = 0.0f;
	float sc = 0.0f;
	float att = 0.0f;

	// Don't forget to normalize L.
	L = normalize(L);

	// The attenuation factor for a point light
	att = 1.0f / quadraticPoly(lightAttenuation.x, lightAttenuation.y, lightAttenuation.z, d);
	
	// dc is the diffuse contribution.
	dc = maxdot(lViewSpaceVertexNormal, L);

	// Since we are in VIEW coordinates, the eye position is at the origin.
	// Therefore the unit direction vector from the point on the surface P
	// to the eye is given by (0 - P) / (||0 - P||) = normalize(-P).
	vec3 E = normalize(-lViewSpaceVertexPosition);

	// We reflect the E direction vector *around* the surface normal.
	// The idea is now that if the angle of incidence of the light is equal
	// to the outgoing angle of incidence to the eye, we experience specularity.
	vec3 R = reflect(E,lViewSpaceVertexNormal);

	// sc is the specular contribution.
	sc = dc > 0.0f ? clamppowmaxdot(R,E, lSpecularColor.a) : 0.0f;
	
	finalColor = lightIntensity.a * att * lDiffuseColor.a * lightIntensity.rgb * (dc * lDiffuseColor.rgb + sc * lSpecularColor.rgb);

	#ifndef NDEBUG
	if (debugFlag == 1)
	{
		finalColor.r += 0.1f;
	}
	else if (debugFlag == 2)
	{
		finalColor.g += 0.1f;
	}
	#endif
}