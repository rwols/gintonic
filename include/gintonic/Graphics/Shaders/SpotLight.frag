//
// SpotLight.frag
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
uniform vec4 lightAttenuation;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightCosineHalfAngle;
uniform int  lightCastShadow;
uniform mat4 lightShadowMatrix;
uniform sampler2DShadow lightShadowDepthTexture;

out vec3 finalColor;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculateScreenPosition()
{
	return vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
}

float quadraticPolynomial(in float constantTerm, in float linearTerm, in float quadraticTerm, in float value)
{
	return constantTerm + linearTerm * value + quadraticTerm * value * value;
}

float quadraticPolynomial(in vec3 coefficients, in float value)
{
	return coefficients.x + coefficients.y * value + coefficients.z * value * value;
}

vec3 getDirection(in vec3 source, in vec3 target)
{
	return target - source;
}

float calculateShadowFactor(in vec3 viewSpacePosition, float cosineTheta)
{
	if (lightCastShadow == 0) return 1.0f;
	// Transform the position from VM-space all the way to PLM-space
	vec4 lClipLightSpacePosition = lightShadowMatrix * vec4(viewSpacePosition, 1.0f);
	lClipLightSpacePosition /= lClipLightSpacePosition.w;

	// Turn the range [-1,1] into the range [0,1]
	lClipLightSpacePosition *= 0.5f;
	lClipLightSpacePosition += 0.5f;

	float lBias = 0.005f * tan(acos(cosineTheta));
	lBias = clamp(lBias, 0.0f, 0.05f);

	lClipLightSpacePosition.z -= lBias;

	// Compare Z-values
	return texture(lightShadowDepthTexture, lClipLightSpacePosition.xyz);
}

#define LIGHT_ATTENUATION lightAttenuation.xyz
#define SPOTLIGHT_EXPONENT lightAttenuation.w
#define LIGHT_INTENSITY lightIntensity.w
#define LIGHT_COLOR lightIntensity.xyz

void main()
{
	vec2 lScreenUV = calculateScreenPosition();

	vec3 lViewSpaceVertexPosition = texture(geometryBufferPositionTexture, lScreenUV).xyz;
	vec4 lDiffuseColor            = texture(geometryBufferDiffuseTexture,  lScreenUV);
	vec4 lSpecularColor           = texture(geometryBufferSpecularTexture, lScreenUV);
	vec3 lViewSpaceVertexNormal   = texture(geometryBufferNormalTexture,   lScreenUV).xyz;
	vec3  lDirectionFromLightToVertex = lViewSpaceVertexPosition - lightPosition;
	float lDistanceFromLightToVertex  = length(lDirectionFromLightToVertex);

	if (lDistanceFromLightToVertex <= 0.0f) discard;

	// Normalize this direction.
	
	lDirectionFromLightToVertex /= lDistanceFromLightToVertex;
	
	vec3 lDirectionFromEyeToLight = -lightDirection;

	float lCosineTheta = dot(lDirectionFromEyeToLight, lViewSpaceVertexNormal);

	if (lCosineTheta < 0.0f) discard;
	
	// float lCosineTheta = max(dot(lDirectionFromEyeToLight, lViewSpaceVertexNormal), 0.0f);

	float lShadowFactor = calculateShadowFactor(lViewSpaceVertexPosition, lCosineTheta);
	
	float lDiffuseFactor = lDiffuseColor.a * lCosineTheta;

	// Determine the spot light effect.
	float lCosineHalfAngle = max(dot(lDirectionFromLightToVertex, lightDirection), 0.0f);
	if (lCosineHalfAngle + lightCosineHalfAngle < 1.0f) discard;
	float lSpotlightEffect = pow(lCosineHalfAngle, SPOTLIGHT_EXPONENT);
	
	// Determine the attenuation factor.
	float lAttenuationFactor = quadraticPolynomial(LIGHT_ATTENUATION, lDistanceFromLightToVertex);

	// Determine the specular factor.
	vec3 lDirectionFromVertexToEye = normalize(-lViewSpaceVertexPosition);
	vec3 lReflection = reflect(lDirectionFromVertexToEye, lViewSpaceVertexNormal);
	float lReflectionCosineAngle = dot(lDirectionFromVertexToEye, lReflection);
	float lSpecularFactor = pow(max(lReflectionCosineAngle, 0.0f), lSpecularColor.a);

	// Compose the final color
	finalColor  = lDiffuseFactor * lDiffuseColor.rgb;
	finalColor += lSpecularFactor * lSpecularColor.rgb;
	finalColor *= lDiffuseColor.a;

	finalColor /= lAttenuationFactor;
	finalColor *= lSpotlightEffect;
	finalColor *= lShadowFactor;
	
	finalColor *= LIGHT_COLOR;
	finalColor *= LIGHT_INTENSITY;
	
	#ifndef NDEBUG

	if (debugFlag == 1)
	{
		finalColor.r += 0.1f;
	}
	else if (debugFlag == 2)
	{
		finalColor.g += 0.1f;
	}
	else if (debugFlag == 3)
	{
		finalColor = vec3(calculateShadowFactor(lDirectionFromLightToVertex, lCosineTheta));
		finalColor += vec3(lightCosineHalfAngle);
	}
	else if (debugFlag == 4)
	{
		finalColor  = lDiffuseColor.rgb;
		finalColor *= lSpecularColor.rgb;
		finalColor *= lDiffuseColor.a;
		finalColor *= lSpotlightEffect;
		finalColor /= lAttenuationFactor;
		finalColor *= LIGHT_COLOR;
		finalColor *= LIGHT_INTENSITY;
	}
	else if (debugFlag == 5)
	{
		finalColor = vec3(1.0f, 0.0f, 0.0f);
	}

	#endif

}