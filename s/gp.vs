//
// gp.vs
//

#version 330

#define GT_VERTEX_LAYOUT_POSITION 0
#define GT_VERTEX_LAYOUT_TEXCOORD 1
#define GT_VERTEX_LAYOUT_NORMAL 2
#define GT_VERTEX_LAYOUT_PVM_MATRIX 3 // 4 5 6
#define GT_VERTEX_LAYOUT_VM_MATRIX 7 // 8 9 10
#define GT_VERTEX_LAYOUT_N_MATRIX 11 // 12 13
#define GT_VERTEX_LAYOUT_FREE_14 14
#define GT_VERTEX_LAYOUT_FREE_15 15

layout(location = GT_VERTEX_LAYOUT_POSITION)   in vec4 in_position;
layout(location = GT_VERTEX_LAYOUT_TEXCOORD)   in vec4 in_texcoord;
//layout(location = GT_VERTEX_LAYOUT_NORMAL)   in vec4 in_tangent;
layout(location = GT_VERTEX_LAYOUT_PVM_MATRIX) in mat4 in_matrix_PVM;
layout(location = GT_VERTEX_LAYOUT_VM_MATRIX)  in mat4 in_matrix_VM;
layout(location = GT_VERTEX_LAYOUT_N_MATRIX)   in mat3 in_matrix_N;

// uniform mat4 matrix_PVM;
// uniform mat4 matrix_VM;
// uniform mat3 matrix_N;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

void main()
{
	vec4 P = vec4(in_position.xyz, 1.0f);
	vec3 N = vec3(in_position.w, in_texcoord.zw);

	gl_Position = in_matrix_PVM * P;
	v_position = (in_matrix_VM * P).xyz;
	v_normal = in_matrix_N * N;

	v_texcoord = in_texcoord.xy;
}