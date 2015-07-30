//
// null.fs
//
// Does absolutely nothing. Useful for stencil operations.
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_TEXCOORD 4
#define GBUFFER_COUNT 5

void main()
{
	discard;
}
