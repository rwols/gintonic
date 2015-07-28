//
// lp_directional.vs
//
// Directional lighting
//

#version 330 core

layout (location = 0) in vec3 model_position;

void main()
{
	gl_Position = vec4(model_position, 1.0f);
}