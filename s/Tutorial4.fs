//
// Tutorial4.fs
//

#version 330 core

in vec4 vColor;

out vec3 fragColor;

void main()
{
	fragColor = vColor.xyz;
}