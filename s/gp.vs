//
// gp.vs
//

#version 330

#define GT_VERTEX_LAYOUT_POSITION 0
#define GT_VERTEX_LAYOUT_TEXCOORD 1
#define GT_VERTEX_LAYOUT_NORMAL 2
#define GT_VERTEX_LAYOUT_PVM_MATRIX 3 // 4 5 6
#define GT_VERTEX_LAYOUT_VM_MATRIX 7 // 8 9
#define GT_VERTEX_LAYOUT_N_MATRIX 10 // 11 12

layout(location = GT_VERTEX_LAYOUT_POSITION)   in vec4   in_position;
layout(location = GT_VERTEX_LAYOUT_TEXCOORD)   in vec4   in_texcoord;
//layout(location = GT_VERTEX_LAYOUT_NORMAL)   in vec4   in_tangent;
layout(location = GT_VERTEX_LAYOUT_PVM_MATRIX) in mat4   in_matrix_PVM;
layout(location = GT_VERTEX_LAYOUT_VM_MATRIX)  in mat3x4 in_matrix_VM;
layout(location = GT_VERTEX_LAYOUT_N_MATRIX)   in mat3   in_matrix_N;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

uniform bool instanced_drawing = true;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

void main()
{
	vec4 P = vec4(in_position.xyz, 1.0f);
	vec3 N = vec3(in_position.w, in_texcoord.zw);

	if (instanced_drawing)
	{
		gl_Position = in_matrix_PVM * P;
		v_position = transpose(in_matrix_VM) * P;
		v_normal = in_matrix_N * N;
	}
	else
	{
		gl_Position = matrix_PVM * P;
		v_position = (matrix_VM * P).xyz;
		v_normal = matrix_N * N;
	}


	v_texcoord = in_texcoord.xy;
}