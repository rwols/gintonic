//
// LightPassPVM.vert
//
// Used by:
//
// * PointLight.frag
// * SpotLight.frag
// * Null.frag
//

#version 330 core

layout (location = 0) in vec4 iSlot0;

uniform mat4 matrix_PVM;

void main()
{
	gl_Position = matrix_PVM * vec4(iSlot0.xyz, 1.0f);
}