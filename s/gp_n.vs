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

layout(location = GINTONIC_VERTEX_LAYOUT_POSITION) in vec3 in_position;
layout(location = GINTONIC_VERTEX_LAYOUT_TEXCOORD) in vec2 in_texcoord;
layout(location = GINTONIC_VERTEX_LAYOUT_NORMAL)   in vec3 in_normal;
layout(location = GINTONIC_VERTEX_LAYOUT_TANGENT)  in vec3 in_tangent;
layout(location = GINTONIC_VERTEX_LAYOUT_BITANGENT) in vec3 in_bitangent;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 v_position; // in VIEW space
out vec2 v_texcoord;
out mat3 v_matrix_T; // TANGENT->VIEW matrix

void main()
{
	// Do the usual calculations
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	v_position = (matrix_VM * vec4(in_position, 1.0f)).xyz;
	v_texcoord = in_texcoord;

	// Transform the normal, tangent and bitangent vectors to VIEW space
	vec3 N = normalize(matrix_N * in_normal);
	vec3 T = normalize(matrix_N * in_tangent);
	vec3 B = normalize(matrix_N * in_bitangent);

	// Gramm-Schmidt orthonormalization
	// Not sure if this is really necessary...
	// vec3 T = normalize(T - dot(N,T) * N);
	// vec3 B = normalize(B - dot(N,B) * N - dot(T,B) * T);
	
	// matrix that goes from VIEW space to TANGENT space
	v_matrix_T = mat3(N, T, B);

	// v_matrix_T is an orthonormal matrix, so its inverse is equal to
	// its transpose. The resulting matrix goes from TANGENT space
	// to VIEW space.
	v_matrix_T = transpose(v_matrix_T);
}