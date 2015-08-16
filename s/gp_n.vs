//
// gp_n.vs
//

/**
 * REMARKS: There's an optimization opportunity here.
 * We could supply the normal vector as-is, put the x and y
 * components of the tangent vector inside the z and w coordinates
 * of texcoord and put the sign of the bitangent vector inside
 * the w coordinate of in_position. We assume that the normal,
 * tangent and bitangent vectors are of unit length and that they
 * form an orthonormal basis so we can calculate
 * the bitangent vector in the vertex shader from this info.
 */

#version 330

#define GINTONIC_VERTEX_LAYOUT_POSITION 0
#define GINTONIC_VERTEX_LAYOUT_TEXCOORD 1
#define GINTONIC_VERTEX_LAYOUT_NORMAL 2
#define GINTONIC_VERTEX_LAYOUT_TANGENT 3
#define GINTONIC_VERTEX_LAYOUT_BITANGENT 4
#define GINTONIC_VERTEX_LAYOUT_COLOR 5

layout(location = GINTONIC_VERTEX_LAYOUT_POSITION) in vec4 in_position;
layout(location = GINTONIC_VERTEX_LAYOUT_TEXCOORD) in vec4 in_texcoord;
layout(location = GINTONIC_VERTEX_LAYOUT_NORMAL) in vec4 in_normal;
// layout(location = GINTONIC_VERTEX_LAYOUT_TANGENT)  in vec3 in_tangent;
// layout(location = GINTONIC_VERTEX_LAYOUT_BITANGENT) in vec3 in_bitangent;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 v_position; // in VIEW space
out vec2 v_texcoord;
out mat3 v_matrix_T; // TANGENT->VIEW matrix

void main()
{
	// Retrieve the packed data.
	vec4 P = vec4(in_position.xyz, 1.0f);
	vec3 N = vec3(in_position.w, in_texcoord.zw);
	vec3 T = in_normal.xyz;
	vec3 B = in_normal.w * cross(N, T);

	// Do the usual calculations
	gl_Position = matrix_PVM * P;
	v_position = (matrix_VM * P).xyz;
	v_texcoord = in_texcoord.xy;

	// Transform the normal, tangent and bitangent vectors
	// from MODEL (or LOCAL) space to VIEW (or CAMERA) space
	T = matrix_N * T;
	B = matrix_N * B;
	N = matrix_N * N;

	// Gramm-Schmidt orthonormalization
	// Not sure if this is really necessary...
	// T = normalize(T - dot(N,T) * N);
	// B = normalize(B - dot(N,B) * N - dot(T,B) * T);

	// matrix that goes from TANGENT space to VIEW space
	v_matrix_T = mat3(T, B, N);
}