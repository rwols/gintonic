//
// gp_null.fs
//
// Geometry Pass: Does nothing but show a pure red color
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_TEXCOORD 4
#define GBUFFER_COUNT 5

in vec3 v_position;
in vec3 v_normal;

layout (location = GBUFFER_POSITION) out vec3 out_position;
layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse;
layout (location = GBUFFER_NORMAL) out vec3 out_normal;

void main()
{
	out_position = v_position;

	// pure red, full ambient color, no diffuse contribution whatsoever.
	out_diffuse = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	
	out_normal = normalize(v_normal);
}
