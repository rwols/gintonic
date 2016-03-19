//
// Skybox.frag
//
// Uses: Skybox.vert

#version 330

in vec3 v_texcoord;

uniform struct Skybox
{
	samplerCube diffuse_texture;
} skybox;

out vec3 final_color;

void main()
{
	final_color = texture(skybox.diffuse_texture, v_texcoord).rgb;
}