//
// gp_cdn.vs
//
// Geometry Pass: Color, Diffuse and Normal
//

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
// uniform mat4 matrix_VM;
// uniform mat3 matrix_N;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

// BIG TODO!

void main()
{
	gl_Position = matrix_PVM * vec4(in_position, 1.0f);
	// v_position  = (matrix_VM * vec4(in_position, 1.0f)).xyz;
	// v_normal    = matrix_N * in_normal;
	v_texcoord  = in_texcoord;

	// Gramm-Schmidt normalization
	vec3 t = in_tangent - dot(in_normal, in_tangent) * in_normal;
	vec3 u = in_bitangent - dot(in_normal, in_bitangent) * in_normal - dot(t, in_bitangent) * t;
	
	// matrix that goes from object space to tangent space
	mat3 matrix_T = transpose(mat3(in_normal, t, u));

	// we supply the normal vector in tangent space
	v_normal = matrix_T * in_normal;

	// the position vector must be supplied in tangent space too
	v_position = matrix_T * in_position;
}