//
// gp_null.vs
//
// Geometry Pass: Does nothing but show a pure red color
//

#version 330

#define GINTONIC_VERTEX_LAYOUT_POSITION 0
#define GINTONIC_VERTEX_LAYOUT_TEXCOORD 1
#define GINTONIC_VERTEX_LAYOUT_NORMAL 2
#define GINTONIC_VERTEX_LAYOUT_TANGENT 3
#define GINTONIC_VERTEX_LAYOUT_BITANGENT 4
#define GINTONIC_VERTEX_LAYOUT_COLOR 5

layout(location = GINTONIC_VERTEX_LAYOUT_POSITION) in vec3 iSlot0;
layout(location = GINTONIC_VERTEX_LAYOUT_NORMAL) in vec3 iSlot1;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 viewPosition; // the view-space position vector
out vec3 viewNormal; // the view-space normal vector

void main()
{
	vec4 localPosition = vec4(iSlot0.xyz, 1.0f);

	gl_Position  = matrix_PVM * localPosition;
	viewPosition = (matrix_VM * localPosition).xyz;
	viewNormal   = matrix_N * iSlot1.xyz;
}