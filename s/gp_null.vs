//
// gp_null.vs
//

#version 330

#define GINTONIC_VERTEX_LAYOUT_POSITION 0
#define GINTONIC_VERTEX_LAYOUT_TEXCOORD 1
#define GINTONIC_VERTEX_LAYOUT_NORMAL 2
#define GINTONIC_VERTEX_LAYOUT_TANGENT 3
#define GINTONIC_VERTEX_LAYOUT_BITANGENT 4
#define GINTONIC_VERTEX_LAYOUT_COLOR 5

layout(location = GINTONIC_VERTEX_LAYOUT_POSITION) in vec3 in_position;
layout(location = GINTONIC_VERTEX_LAYOUT_NORMAL) in vec3 in_normal;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 v_position; // the view-space position vector
out vec3 v_normal; // the view-space normal vector

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	v_position  = (matrix_VM * vec4(in_position, 1.0f)).xyz;
	v_normal    = matrix_N * in_normal;
}