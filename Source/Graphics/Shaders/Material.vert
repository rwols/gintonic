
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

#define GT_MAX_BONES 64

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
uniform int  hasTangentsAndBitangents;
uniform int  instancedRendering;
uniform int  debugFlag;

uniform mat4 matrixB[GT_MAX_BONES];
uniform mat3 matrixBN[GT_MAX_BONES];

out vec3 viewSpaceVertexPosition;
out vec3 viewSpaceVertexNormal;
out vec2 textureCoordinates;
out mat3 tangentMatrix;

vec4 fromBoneSpaceToLocalSpace(vec4 v)
{
	if (iBoneID == ivec4(-1, -1, -1, -1)) return v;
	vec4 lTemp = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	lTemp += (matrixB[iBoneID.x] * v) * iBoneWeight.x;
	lTemp += (matrixB[iBoneID.y] * v) * iBoneWeight.y;
	lTemp += (matrixB[iBoneID.z] * v) * iBoneWeight.z;
	lTemp += (matrixB[iBoneID.w] * v) * iBoneWeight.w;
	return lTemp;
}

vec3 fromBoneSpaceToNormalSpace(vec3 v)
{
	if (iBoneID == ivec4(-1, -1, -1, -1)) return v;
	vec3 lTemp = vec3(0.0f, 0.0f, 0.0f);
	lTemp += (matrixBN[iBoneID.x] * v) * iBoneWeight.x;
	lTemp += (matrixBN[iBoneID.y] * v) * iBoneWeight.y;
	lTemp += (matrixBN[iBoneID.z] * v) * iBoneWeight.z;
	lTemp += (matrixBN[iBoneID.w] * v) * iBoneWeight.w;
	return lTemp;
}

void main()
{
	vec4 localPosition;
	if (debugFlag == 1)
	{
		localPosition = fromBoneSpaceToLocalSpace(vec4(iSlot0.xyz, 1.0f));
	}
	else
	{
		localPosition = vec4(iSlot0.xyz, 1.0f);
	}
	vec3 localNormal;
	if (debugFlag == 1)
	{
		localNormal = fromBoneSpaceToNormalSpace(iSlot1.xyz);
	}
	else
	{
		localNormal = iSlot1.xyz;
	}

	textureCoordinates = vec2(iSlot0.w, iSlot1.w);

	if (instancedRendering != 0)
	{
		gl_Position             =  iMatrixPVM * localPosition;
		viewSpaceVertexPosition = (iMatrixVM  * localPosition).xyz;
		viewSpaceVertexNormal   =  iMatrixN   * localNormal;

		if (hasTangentsAndBitangents != 0)
		{
			vec3 localTangent;
			if (debugFlag == 1)
			{
				localTangent = fromBoneSpaceToNormalSpace(iSlot2.xyz);
			}
			else
			{
				localTangent = iSlot2.xyz;
			}
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
		
		if (hasTangentsAndBitangents != 0)
		{
			vec3 localTangent;
			if (debugFlag == 1)
			{
				localTangent = fromBoneSpaceToNormalSpace(iSlot2.xyz);
			}
			else
			{
				localTangent = iSlot2.xyz;
			}
			vec3 localBitangent = cross(localNormal, localTangent) * iSlot2.w;
			tangentMatrix = mat3(
				matrixN * localTangent, 
				matrixN * localBitangent, 
				matrixN * localNormal);
		}
	}
}