//
// gp_n.vs
//
// Geometry pass vertex shader, non-instanced version, tangents and bitangents.
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
layout(location = GT_VERTEX_LAYOUT_NORMAL)     in vec4 iSlot2;
// layout(location = GT_VERTEX_LAYOUT_PVM_MATRIX) in mat4 in_matrix_PVM;
// layout(location = GT_VERTEX_LAYOUT_VM_MATRIX)  in mat4 in_matrix_VM;
// layout(location = GT_VERTEX_LAYOUT_N_MATRIX)   in mat3 in_matrix_N;

uniform mat4 matrix_PVM;
uniform mat4 matrix_VM;
uniform mat3 matrix_N;

out vec3 viewPosition; // in VIEW space
out vec2 texCoords;
out mat3 tangentMatrix; // TANGENT->VIEW matrix

void main()
{
	// Retrieve the packed data.
	vec4 localPosition  = vec4(iSlot0.xyz, 1.0f);
	vec3 localNormal    = iSlot1.xyz;
	vec3 localTangent   = iSlot2.xyz;
	vec3 localBitangent = iSlot2.w * cross(localNormal, localTangent);
	texCoords = vec2(iSlot0.w, iSlot1.w);

	// Do the usual calculations
	gl_Position  = matrix_PVM * localPosition;
	viewPosition = (matrix_VM * localPosition).xyz;
	
	// Transform the normal, tangent and bitangent vectors
	// from MODEL (or LOCAL) space to VIEW (or CAMERA) space
	localTangent   = matrix_N * localTangent;
	localBitangent = matrix_N * localBitangent;
	localNormal    = matrix_N * localNormal;

	// Gramm-Schmidt orthonormalization
	// Not sure if this is really necessary...
	// T = normalize(T - dot(N,T) * N);
	// B = normalize(B - dot(N,B) * N - dot(T,B) * T);

	// matrix that goes from TANGENT space to VIEW space
	tangentMatrix = mat3(localTangent, localBitangent, localNormal);
}