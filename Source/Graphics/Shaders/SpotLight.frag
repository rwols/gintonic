//
// SpotLight.frag
//
// Uses LightPassPVM.vert
//

#version 330 core

// #define NDEBUG

uniform vec2 viewport_size;

#ifndef NDEBUG
uniform int debugflag;
#endif

uniform struct GeometryBuffer
{
	sampler2D position; // these are in VIEW coordinates
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
} gbuffer;

uniform struct SpotLight {
	vec4 intensity;
	vec3 position; // in VIEW coordinates
	vec3 direction; // in VIEW coordinates
	vec4 attenuation;
	float angle; // in COSINE of RADIANS
} light;

out vec3 final_color;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculateScreenPosition()
{
	return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
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
// 	float angle = dot(directionFromLightToVertex, light.direction);
// 	return (angle >= light.angle) ? pow(maxdot(directionFromLightToVertex, light.direction), light.attenuation.w) : 0.0f;	
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
// 	vec3 lDirectionFromLightToVertex = normalize(viewSpaceVertexPosition - light.position);
// 	float angle = dot(lDirectionFromLightToVertex, light.direction);
// 	return angle >= light.angle ? 1.0f : 0.0f;
// }

// float computeDiffuseFactor(in vec3 viewSpaceVertexPosition)
// {
// 	vec3 lDirectionFromLightToVertex = viewSpaceVertexPosition - light.position;
// 	float lDistanceFromVertexToLight = length(lDirectionFromLightToVertex);

// 	if (lDistanceFromVertexToLight > 0.0f)
// 	{
// 		// Make this direction of unit length
// 		lDirectionFromLightToVertex /= lDistanceFromVertexToLight;

// 		float angle = dot(lDirectionFromLightToVertex, light.direction);
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

	vec3 lViewSpaceVertexPosition = texture(gbuffer.position, lScreenUV).xyz;
	vec4 lDiffuseColor            = texture(gbuffer.diffuse, lScreenUV);
	vec4 lSpecularColor           = texture(gbuffer.specular, lScreenUV);
	vec3 lViewSpaceVertexNormal   = texture(gbuffer.normal, lScreenUV).xyz;

	vec3  lDirectionFromLightToVertex = lViewSpaceVertexPosition - light.position;
	float lDistanceFromLightToVertex  = length(lDirectionFromLightToVertex);

	if (lDistanceFromLightToVertex <= 0.0f) discard;

	// Normalize this direction.
	lDirectionFromLightToVertex /= lDistanceFromLightToVertex;

	// Determine the spot light effect.
	float lCosineAngle = dot(lDirectionFromLightToVertex, light.direction);
	if (lCosineAngle < light.angle) discard;
	float lSpotlightEffect = pow(lCosineAngle, light.attenuation.w);
	
	// Determine the attenuation factor.
	float lAttenuationFactor = quadraticPolynomial(light.attenuation.xyz, lDistanceFromLightToVertex);

	// Determine the specular factor.
	vec3 lDirectionFromVertexToEye = normalize(-lViewSpaceVertexPosition);
	vec3 lReflection = reflect(lDirectionFromVertexToEye, lViewSpaceVertexNormal);
	float lReflectionCosineAngle = dot(lDirectionFromVertexToEye, lReflection);
	float lSpecularFactor = pow(max(lReflectionCosineAngle, 0.0f), lSpecularColor.a);

	// Compose the final color
	final_color  = lDiffuseColor.rgb;
	final_color += lSpecularFactor * lSpecularColor.rgb;
	final_color *= lDiffuseColor.a;
	final_color *= lSpotlightEffect;
	final_color /= lAttenuationFactor;
	final_color *= light.intensity.rgb;
	final_color *= light.intensity.a;
	// final_color *= lDiffuseColor.a;
	
	#ifndef NDEBUG
	if (debugflag == 1)
	{
		final_color.r += 0.1f;
	}
	else if (debugflag == 2)
	{
		final_color.g += 0.1f;
	}
	#endif

}