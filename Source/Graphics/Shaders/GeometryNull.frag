//
// GeometryNull.frag
//
// Geometry Pass: Does nothing but show a pure red color. Uses GeometryNull.vert
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_TEXCOORD 4
#define GBUFFER_COUNT 5

in vec3 viewPosition;
in vec3 viewNormal;

layout (location = GBUFFER_POSITION) out vec3 outPosition;
layout (location = GBUFFER_DIFFUSE) out vec4 outDiffuse;
layout (location = GBUFFER_NORMAL) out vec3 outNormal;

void main()
{
	outPosition = viewPosition;

	// pure red, full ambient color, no diffuse contribution whatsoever.
	outDiffuse = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	
	outNormal = normalize(viewNormal);
}
