//
// Tutorial5.fs
//

#version 330 core

uniform sampler2D MaterialAmbientTexture;
uniform float     MaterialAmbientFactor;

in vec2 vUv;

out vec4 fragColor;

void main()
{
	fragColor = MaterialAmbientFactor * texture(MaterialAmbientTexture, vUv);
}