
#version 330 core

#define GT_VERTEX_LAYOUT_SLOT_0 0         //  pos.X  pos.Y  pos.Z   uv.X
#define GT_VERTEX_LAYOUT_SLOT_1 1         //  nor.X  nor.Y  nor.Z   uv.Y
#define GT_VERTEX_LAYOUT_SLOT_2 2         //  tan.X  tan.Y  tan.Z   hand
#define GT_VERTEX_LAYOUT_SLOT_3_4_5_6 3   // PVM.00 PVM.01 PVM.02 PVM.03 <--- instanced rendering
                                          // PVM.10 PVM.11 PVM.12 PVM.13 <--- instanced rendering
                                          // PVM.20 PVM.21 PVM.22 PVM.23 <--- instanced rendering
                                          // PVM.30 PVM.31 PVM.32 PVM.33 <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_7_8_9_10 7  //  VM.00  VM.01  VM.02  VM.03 <--- instanced rendering
                                          //  VM.10  VM.11  VM.12  VM.13 <--- instanced rendering
                                          //  VM.20  VM.21  VM.22  VM.23 <--- instanced rendering
                                          //  VM.30  VM.31  VM.32  VM.33 <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_11_12_13 11 //   N.00   N.01   N.02 unused <--- instanced rendering
                                          //   N.10   N.11   N.12 unused <--- instanced rendering
                                          //   N.20   N.21   N.22 unused <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_14 14       // boneID.x boneID.y boneID.z boneID.w
#define GT_VERTEX_LAYOUT_SLOT_15 15       // weight.x weight.y weight.z weight.w

#define GT_SKELETON_MAX_JOINTS (1 << 7)
#define GT_JOINT_NONE ((GT_SKELETON_MAX_JOINTS) - 1)

#define HAS_DIFFUSE_TEXTURE         1
#define HAS_SPECULAR_TEXTURE        2
#define HAS_NORMAL_TEXTURE          4
#define HAS_TANGENTS_AND_BITANGENTS 8
#define MESH_HAS_JOINTS             16
#define INSTANCED_RENDERING         32

layout(location = GT_VERTEX_LAYOUT_SLOT_0)        in vec4  iSlot0;
layout(location = GT_VERTEX_LAYOUT_SLOT_1)        in vec4  iSlot1;
layout(location = GT_VERTEX_LAYOUT_SLOT_2)        in vec4  iSlot2;
layout(location = GT_VERTEX_LAYOUT_SLOT_3_4_5_6)  in mat4  iMatrixPVM;
layout(location = GT_VERTEX_LAYOUT_SLOT_7_8_9_10) in mat4  iMatrixVM;
layout(location = GT_VERTEX_LAYOUT_SLOT_11_12_13) in mat3  iMatrixN;
layout(location = GT_VERTEX_LAYOUT_SLOT_14)       in ivec4 iBoneID;
layout(location = GT_VERTEX_LAYOUT_SLOT_15)       in vec4  iBoneWeight;

uniform mat4 matrixPVM;
uniform mat4 matrixVM;
uniform mat3 matrixN;
uniform mat4 matrixB[GT_SKELETON_MAX_JOINTS];
uniform mat3 matrixBN[GT_SKELETON_MAX_JOINTS];
uniform int  materialFlag;

// uniform Matrix44Block
// {
// 	mat4 projection;
// 	mat4 view;
// 	mat4 projectionView;
// } matrix44;

// uniform Joint44
// {
// 	mat4 matrix[GT_SKELETON_MAX_JOINTS];
// } joint;

// uniform Joint33
// {
// 	vec4 matrix[3 * GT_SKELETON_MAX_JOINTS];
// } normalJoint;

// uniform Matrix44
// {
// 	uniform mat4 matrixPVM;
// 	uniform mat4 matrixVM;
// 	uniform mat4 matrixB[GT_SKELETON_MAX_JOINTS];
// } matrix44;

// uniform Matrix33
// {
// 	uniform mat3 matrixN;
// 	uniform mat3 matrixBN[GT_SKELETON_MAX_JOINTS];
// } matrix33;

out vec3 viewSpaceVertexPosition;
out vec3 viewSpaceVertexNormal;
out vec2 textureCoordinates;
out mat3 tangentMatrix;

#define checkFlag(flagID) (materialFlag & flagID) == flagID

vec4 fromBoneSpaceToLocalSpace(in vec4 v)
{
	vec4 lTemp = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	lTemp += (matrixB[iBoneID.x] * v) * iBoneWeight.x;
	lTemp += (matrixB[iBoneID.y] * v) * iBoneWeight.y;
	lTemp += (matrixB[iBoneID.z] * v) * iBoneWeight.z;
	lTemp += (matrixB[iBoneID.w] * v) * iBoneWeight.w;

	// lTemp += (joint.matrix[iBoneID.x] * v) * iBoneWeight.x;
	// lTemp += (joint.matrix[iBoneID.y] * v) * iBoneWeight.y;
	// lTemp += (joint.matrix[iBoneID.z] * v) * iBoneWeight.z;
	// lTemp += (joint.matrix[iBoneID.w] * v) * iBoneWeight.w;

	return lTemp;
}

vec3 fromBoneSpaceToNormalSpace(in vec3 v)
{
	vec3 lTemp = vec3(0.0f, 0.0f, 0.0f);

	lTemp += (matrixBN[iBoneID.x] * v) * iBoneWeight.x;
	lTemp += (matrixBN[iBoneID.y] * v) * iBoneWeight.y;
	lTemp += (matrixBN[iBoneID.z] * v) * iBoneWeight.z;
	lTemp += (matrixBN[iBoneID.w] * v) * iBoneWeight.w;
	
	// mat3 lMatrixX = transpose(inverse(mat3(joint.matrix[iBoneID.x])));
	// mat3 lMatrixY = transpose(inverse(mat3(joint.matrix[iBoneID.y])));
	// mat3 lMatrixZ = transpose(inverse(mat3(joint.matrix[iBoneID.z])));
	// mat3 lMatrixW = transpose(inverse(mat3(joint.matrix[iBoneID.w])));

	// lTemp += (lMatrixX * v) * iBoneWeight.x;
	// lTemp += (lMatrixY * v) * iBoneWeight.y;
	// lTemp += (lMatrixZ * v) * iBoneWeight.z;
	// lTemp += (lMatrixW * v) * iBoneWeight.w;

	// lTemp += (normalJoint.matrix[iBoneID.x] * v) * iBoneWeight.x;
	// lTemp += (normalJoint.matrix[iBoneID.y] * v) * iBoneWeight.y;
	// lTemp += (normalJoint.matrix[iBoneID.z] * v) * iBoneWeight.z;
	// lTemp += (normalJoint.matrix[iBoneID.w] * v) * iBoneWeight.w;

	return lTemp;
}

void main()
{
	
	bool lHasJoints     = checkFlag(MESH_HAS_JOINTS);
	vec4 localPosition  = lHasJoints ? fromBoneSpaceToLocalSpace(vec4(iSlot0.xyz, 1.0f)) : vec4(iSlot0.xyz, 1.0f);
	vec3 localNormal    = lHasJoints ? fromBoneSpaceToNormalSpace(iSlot1.xyz) : iSlot1.xyz;
	textureCoordinates  = vec2(iSlot0.w, iSlot1.w);

	if (checkFlag(INSTANCED_RENDERING))
	{
		gl_Position             =  iMatrixPVM * localPosition;
		viewSpaceVertexPosition = (iMatrixVM  * localPosition).xyz;
		viewSpaceVertexNormal   =  iMatrixN   * localNormal;

		if (checkFlag(HAS_TANGENTS_AND_BITANGENTS))
		{
			vec3 localTangent   = lHasJoints ? fromBoneSpaceToNormalSpace(iSlot2.xyz) : iSlot2.xyz;
			vec3 localBitangent = cross(localNormal, localTangent) * iSlot2.w;
			
			tangentMatrix = mat3(
				iMatrixN * localTangent, 
				iMatrixN * localBitangent, 
				iMatrixN * localNormal);
		}
	}
	else
	{
		gl_Position             =  matrixPVM * localPosition;
		viewSpaceVertexPosition = (matrixVM  * localPosition).xyz;
		viewSpaceVertexNormal   =  matrixN   * localNormal;
		
		if (checkFlag(HAS_TANGENTS_AND_BITANGENTS))
		{
			vec3 localTangent   = lHasJoints ? fromBoneSpaceToNormalSpace(iSlot2.xyz) : iSlot2.xyz;
			vec3 localBitangent = cross(localNormal, localTangent) * iSlot2.w;

			tangentMatrix = mat3(
				matrixN * localTangent, 
				matrixN * localBitangent, 
				matrixN * localNormal);
		}
	}
}
