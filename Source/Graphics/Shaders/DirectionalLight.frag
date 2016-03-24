//
// DirectionalLight.frag
//
// Uses LightPass.vert
//

#version 330 core

#define SHADOW_FACTOR 0.5f

uniform vec2 viewportSize;

uniform sampler2D geometryBufferPositionTexture;
uniform sampler2D geometryBufferDiffuseTexture;
uniform sampler2D geometryBufferSpecularTexture;
uniform sampler2D geometryBufferNormalTexture;

uniform vec4 lightIntensity;
uniform vec3 lightDirection;
uniform int  lightCastShadow;
uniform mat4 lightShadowMatrix;
uniform sampler2D lightShadowDepthTexture;

out vec3 finalColor;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculateScreenPosition()
{
	return vec2(gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
}

float calculateShadowFactor(in vec3 viewSpacePosition)
{
	if (lightCastShadow == 0) return 1.0f;

	// Transform the position from VM-space all the way to PLM-space
	vec4 lClipLightSpacePosition = lightShadowMatrix * vec4(viewSpacePosition, 1.0f);
	lClipLightSpacePosition /= lClipLightSpacePosition.w;
	// lClipLightSpacePosition *= 0.5f;
	// lClipLightSpacePosition += 0.5f;

	// Build UV coordinate to sample the texture with
	// vec2 lTextureCoordinates;
	// lTextureCoordinates.x = 0.5f * lClipLightSpacePosition.x + 0.5f;
	// lTextureCoordinates.y = 0.5f * lClipLightSpacePosition.y + 0.5f;
	// float lEyeDepth = 0.5f * lClipLightSpacePosition.z + 0.5f;

	vec2 lTexCoords;
	lTexCoords.x = 0.5f * lClipLightSpacePosition.x + 0.5f;
	lTexCoords.y = 0.5f * lClipLightSpacePosition.y + 0.5f;

	float lEyeDepth = 0.5f * lClipLightSpacePosition.z + 0.5f;

	// Compare Z-values
	float lLightDepth = texture(lightShadowDepthTexture, lTexCoords).x;
	if (lLightDepth < lEyeDepth + 0.01f) return SHADOW_FACTOR;
	else return 1.0f;
}

void main()
{
	vec2 lScreenUV = calculateScreenPosition();

	vec3 lViewSpaceVertexPosition = texture(geometryBufferPositionTexture, lScreenUV).xyz;
	vec4 lDiffuseColor            = texture(geometryBufferDiffuseTexture,  lScreenUV);
	vec4 lSpecularColor           = texture(geometryBufferSpecularTexture, lScreenUV);
	vec3 lViewSpaceVertexNormal   = texture(geometryBufferNormalTexture,   lScreenUV).xyz;

	float lShadowFactor = calculateShadowFactor(lViewSpaceVertexPosition);

	vec3 lDirectionFromEyeToLight = normalize(-lightDirection);

	float lDiffuseFactor = lDiffuseColor.a * max(dot(lDirectionFromEyeToLight, lViewSpaceVertexNormal), 0.0f);

	vec3 E = normalize(-lViewSpaceVertexPosition);
 	
 	vec3 R = normalize(-reflect(lDirectionFromEyeToLight,lViewSpaceVertexNormal));
	
	float lSpecularFactor = pow(max(dot(R,E), 0.0f), lSpecularColor.a);
	
	lSpecularFactor = clamp(lSpecularFactor, 0.0f, 1.0f); 

	finalColor  = lDiffuseFactor * lDiffuseColor.rgb;
	finalColor += lSpecularFactor * lSpecularColor.rgb;
	finalColor *= lShadowFactor;
	finalColor *= lightIntensity.a;
	finalColor *= lightIntensity.rgb;

	// finalColor = lightIntensity.a * lDiffuseFactor * lightIntensity.rgb * lDiffuseColor.rgb 
	// 	+ lSpecularFactor * lSpecularColor.rgb * lightIntensity.rgb;

	// finalColor *= lShadowFactor;
}