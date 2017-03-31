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

uniform mat4 matrixPVM;

void main()
{
	gl_Position = matrixPVM * vec4(iSlot0.xyz, 1.0f);
}