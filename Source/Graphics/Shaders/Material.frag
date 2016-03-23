
#version 330 core

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3

#define HAS_DIFFUSE_TEXTURE 1
#define HAS_SPECULAR_TEXTURE 2
#define HAS_NORMAL_TEXTURE 4

in vec3 viewSpaceVertexPosition;
in vec3 viewSpaceVertexNormal;
in vec2 textureCoordinates;
in mat3 tangentMatrix;

uniform vec4      materialDiffuseColor;
uniform vec4      materialSpecularColor;
uniform sampler2D materialDiffuseTexture;
uniform sampler2D materialSpecularTexture;
uniform sampler2D materialNormalTexture;
uniform int       materialFlag;
uniform int       hasTangentsAndBitangents;

layout(location = GBUFFER_POSITION) out vec3 outPosition;
layout(location = GBUFFER_DIFFUSE)  out vec4 outDiffuse;
layout(location = GBUFFER_SPECULAR) out vec4 outSpecular;
layout(location = GBUFFER_NORMAL)   out vec3 outNormal;

void main()
{
	outPosition = viewSpaceVertexPosition;

	outDiffuse = materialDiffuseColor;

	if ((materialFlag & HAS_DIFFUSE_TEXTURE) == HAS_DIFFUSE_TEXTURE)
	{
		outDiffuse *= texture(materialDiffuseTexture, textureCoordinates);
	}

	outSpecular = materialSpecularColor;

	if ((materialFlag & HAS_SPECULAR_TEXTURE) == HAS_SPECULAR_TEXTURE)
	{
		outSpecular *= texture(materialSpecularTexture, textureCoordinates);
	}

	if (hasTangentsAndBitangents != 0 && (materialFlag & HAS_NORMAL_TEXTURE) == HAS_NORMAL_TEXTURE)
	{
		vec3 lSampledNormal = texture(materialNormalTexture, textureCoordinates).xyz;
		lSampledNormal = 2.0f * lSampledNormal - 1.0f;
		
		outNormal = normalize(tangentMatrix * lSampledNormal);
	}
	else
	{
		outNormal = viewSpaceVertexNormal;
	}
}