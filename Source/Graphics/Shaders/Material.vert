
#version 330 core

#define GT_VERTEX_LAYOUT_SLOT_0 0   //  pos.X  pos.Y  pos.Z   uv.X
#define GT_VERTEX_LAYOUT_SLOT_1 1   //  nor.X  nor.Y  nor.Z   uv.Y
#define GT_VERTEX_LAYOUT_SLOT_2 2   //  tan.X  tan.Y  tan.Z   hand
#define GT_VERTEX_LAYOUT_SLOT_3 3   // PVM.00 PVM.01 PVM.02 PVM.03 <--- instanced rendering
                                    // PVM.10 PVM.11 PVM.12 PVM.13 <--- instanced rendering
                                    // PVM.20 PVM.21 PVM.22 PVM.23 <--- instanced rendering
                                    // PVM.30 PVM.31 PVM.32 PVM.33 <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_7 7   //  VM.00  VM.01  VM.02  VM.03 <--- instanced rendering
                                    //  VM.10  VM.11  VM.12  VM.13 <--- instanced rendering
                                    //  VM.20  VM.21  VM.22  VM.23 <--- instanced rendering
                                    //  VM.30  VM.31  VM.32  VM.33 <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_11 11 //   N.00   N.01   N.02 unused <--- instanced rendering
                                    //   N.10   N.11   N.12 unused <--- instanced rendering
                                    //   N.20   N.21   N.22 unused <--- instanced rendering
#define GT_VERTEX_LAYOUT_SLOT_14 14 //   free   free   free   free
#define GT_VERTEX_LAYOUT_SLOT_15 15 //   free   free   free   free

layout(location = GT_VERTEX_LAYOUT_SLOT_0)  in vec4 iSlot0;
layout(location = GT_VERTEX_LAYOUT_SLOT_1)  in vec4 iSlot1;
layout(location = GT_VERTEX_LAYOUT_SLOT_2)  in vec4 iSlot2;
layout(location = GT_VERTEX_LAYOUT_SLOT_3)  in mat4 iMatrixPVM;
layout(location = GT_VERTEX_LAYOUT_SLOT_7)  in mat4 iMatrixVM;
layout(location = GT_VERTEX_LAYOUT_SLOT_11) in mat3 iMatrixN;

uniform mat4 matrixPVM;
uniform mat4 matrixVM;
uniform mat3 matrixN;
uniform int  hasTangentsAndBitangents;
uniform int  instancedRendering;

out vec3 viewSpaceVertexPosition;
out vec3 viewSpaceVertexNormal;
out vec2 textureCoordinates;
out mat3 tangentMatrix;

void main()
{
	vec4 localPosition = vec4(iSlot0.xyz, 1.0f);
	vec3 localNormal   = iSlot1.xyz;
	textureCoordinates = vec2(iSlot0.w, iSlot1.w);

	if (instancedRendering)
	{
		gl_Position = iMatrixPVM * localPosition;
		viewSpaceVertexPosition = (iMatrixVM * localPosition).xyz;
		if (hasTangentsAndBitangents)
		{
			vec3 localTangent = vec3(iSlot2.xyz);
			vec3 localBitangent = cross(localNormal, localTangent) * iSlot2.w;
			tangentMatrix = mat3(
				iMatrixN * localTangent, 
				iMatrixN * localBitangent, 
				iMatrixN * localNormal);
		}
		else
		{
			viewSpaceVertexNormal = iMatrixN * localNormal;
		}
	}
	else
	{
		gl_Position = matrixPVM * localPosition;
		viewSpaceVertexPosition = (matrixVM * localPosition).xyz;
		if (hasTangentsAndBitangents)
		{
			vec3 localTangent = vec3(iSlot2.xyz);
			vec3 localBitangent = cross(localNormal, localTangent) * iSlot2.w;
			tangentMatrix = mat3(
				matrixN * localTangent, 
				matrixN * localBitangent, 
				matrixN * localNormal);
		}
		else
		{
			viewSpaceVertexNormal = matrixN * localNormal;
		}
	}
}