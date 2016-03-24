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

out vec3 finalColor;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculateScreenPosition()
{
	return vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
}

// float maxdot(in vec3 A, in vec3 B)
// {
// 	return max(dot(A,B), 0.0f);
// }

// float clamppowmaxdot(in vec3 A, in vec3 B, in float exponent)
// {
// 	// return pow(max(dot(A,B), 0.0f), exponent);
// 	return clamp(pow(maxdot(A,B), exponent), 0.0f, 1.0f);
// }

float quadraticPolynomial(in float constantTerm, in float linearTerm, in float quadraticTerm, in float value)
{
	return constantTerm + linearTerm * value + quadraticTerm * value * value;
}

float quadraticPolynomial(in vec3 coefficients, in float value)
{
	return coefficients.x + coefficients.y * value + coefficients.z * value * value;
}

// float spotlightfactor(in vec3 directionFromLightToVertex)
// {
// 	float angle = dot(directionFromLightToVertex, lightDirection);
// 	return (angle >= light.angle) ? pow(maxdot(directionFromLightToVertex, lightDirection), light.attenuation.w) : 0.0f;	
// }

// float computeAttenuationFactor(in float distanceFromVertexToLight)
// {
// 	if (distanceFromVertexToLight > 0.0f)
// 	{
// 		1.0f / quadraticPolynomial(light.attenuation.x, light.attenuation.y, light.attenuation.z, distanceFromVertexToLight);
// 	}
// 	else
// 	{
// 		return 0.0f;
// 	}
// }

// float computeSpotlightFactor(in vec3 viewSpaceVertexPosition)
// {
// 	vec3 lDirectionFromLightToVertex = normalize(viewSpaceVertexPosition - lightPosition);
// 	float angle = dot(lDirectionFromLightToVertex, lightDirection);
// 	return angle >= light.angle ? 1.0f : 0.0f;
// }

// float computeDiffuseFactor(in vec3 viewSpaceVertexPosition)
// {
// 	vec3 lDirectionFromLightToVertex = viewSpaceVertexPosition - lightPosition;
// 	float lDistanceFromVertexToLight = length(lDirectionFromLightToVertex);

// 	if (lDistanceFromVertexToLight > 0.0f)
// 	{
// 		// Make this direction of unit length
// 		lDirectionFromLightToVertex /= lDistanceFromVertexToLight;

// 		float angle = dot(lDirectionFromLightToVertex, lightDirection);
// 		if (angle >= light.angle)
// 		{
// 			float spotlightEffect = pow(max(angle, 0.0f), light.attenuation.w);
			
// 			float attenuationEffect = quadraticPolynomial(
// 				light.attenuation.x, 
// 				light.attenuation.y, 
// 				light.attenuation.z, 
// 				lDistanceFromVertexToLight);

// 			return spotlightEffect / attenuationEffect;
// 		}
// 		else
// 		{
// 			return 0.0f;
// 		}
// 	}
// 	else
// 	{
// 		return 0.0f;
// 	}
// }

// float computeSpecularFactor(
// 	in vec3 viewSpaceVertexPosition, 
// 	in vec3 viewSpaceVertexNormal, 
// 	in float specularExponent)
// {
// 	vec3 lDirectionFromVertexToEye = normalize(-viewSpaceVertexPosition);
// 	vec3 lReflection = reflect(lDirectionFromVertexToEye, viewSpaceVertexNormal);
// 	float lCosineAngle = dot(lDirectionFromVertexToEye, lReflection);
// 	return pow(max(lCosineAngle, 0.0f), specularExponent);
// }

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

	// Determine the spot light effect.
	float lCosineAngle = dot(lDirectionFromLightToVertex, lightDirection);
	if (lCosineAngle < lightCosineHalfAngle) discard;
	float lSpotlightEffect = pow(lCosineAngle, lightAttenuation.w);
	
	// Determine the attenuation factor.
	float lAttenuationFactor = quadraticPolynomial(lightAttenuation.xyz, lDistanceFromLightToVertex);

	// Determine the specular factor.
	vec3 lDirectionFromVertexToEye = normalize(-lViewSpaceVertexPosition);
	vec3 lReflection = reflect(lDirectionFromVertexToEye, lViewSpaceVertexNormal);
	float lReflectionCosineAngle = dot(lDirectionFromVertexToEye, lReflection);
	float lSpecularFactor = pow(max(lReflectionCosineAngle, 0.0f), lSpecularColor.a);

	// Compose the final color
	finalColor  = lDiffuseColor.rgb;
	finalColor += lSpecularFactor * lSpecularColor.rgb;
	finalColor *= lDiffuseColor.a;
	finalColor *= lSpotlightEffect;
	finalColor /= lAttenuationFactor;
	finalColor *= lightIntensity.rgb;
	finalColor *= lightIntensity.a;
	// finalColor *= lDiffuseColor.a;
	
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