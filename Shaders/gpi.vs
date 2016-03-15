//
// gpi.vs
//
// Geometry pass vertex shader, instanced version, no tangents and bitangents.
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

layout(location = GT_VERTEX_LAYOUT_POSITION)   in vec4 iSlot0;
layout(location = GT_VERTEX_LAYOUT_TEXCOORD)   in vec4 iSlot1;
// layout(location = GT_VERTEX_LAYOUT_NORMAL)     in vec4 in_tangent;
layout(location = GT_VERTEX_LAYOUT_PVM_MATRIX) in mat4 iMatrix_PVM;
layout(location = GT_VERTEX_LAYOUT_VM_MATRIX)  in mat4 iMatrix_VM;
layout(location = GT_VERTEX_LAYOUT_N_MATRIX)   in mat3 iMatrix_N;

// uniform mat4 matrix_PVM;
// uniform mat4 matrix_VM;
// uniform mat3 matrix_N;

out vec3 viewPosition;
out vec2 texCoords;
out vec3 viewNormal;

void main()
{
	vec4 localPosition = vec4(iSlot0.xyz, 1.0f);
	vec3 localNormal = iSlot0.xyz;

	gl_Position = iMatrix_PVM * localPosition;
	viewPosition = (iMatrix_VM * localPosition).xyz;
	viewNormal = iMatrix_N * localNormal;

	texCoords = vec2(iSlot0.w, iSlot1.w);
}