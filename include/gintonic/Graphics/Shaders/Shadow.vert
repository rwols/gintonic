
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
layout(location = GT_VERTEX_LAYOUT_SLOT_3)  in mat4 iMatrixPVM;

uniform mat4 matrixPVM;
uniform int  instancedRendering;

void main()
{
	vec4 localPosition = vec4(iSlot0.xyz, 1.0f);
      if (instancedRendering != 0)
      {
            gl_Position = iMatrixPVM * localPosition;
      }
      else
      {
            gl_Position = matrixPVM * localPosition;
      }
}