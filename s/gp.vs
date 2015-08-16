//
// gp.vs
//

#version 330

#define GINTONIC_VERTEX_LAYOUT_POSITION 0
#define GINTONIC_VERTEX_LAYOUT_TEXCOORD 1
#define GINTONIC_VERTEX_LAYOUT_NORMAL 2
#define GINTONIC_VERTEX_LAYOUT_TANGENT 3
#define GINTONIC_VERTEX_LAYOUT_BITANGENT 4
#define GINTONIC_VERTEX_LAYOUT_COLOR 5

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_texcoord;
// layout(location = 2) in vec4 in_tangent;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

void main()
{
	vec4 P = vec4(in_position.xyz, 1.0f);
	vec3 N = vec3(in_position.w, in_texcoord.zw);

	gl_Position = matrix_PVM * P;
	v_position = (matrix_VM * P).xyz;

	v_normal = matrix_N * N;
	v_texcoord = in_texcoord.xy;
}