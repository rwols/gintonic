//
// light_pass.vs
//

#version 330 core

precision mediump float;

layout (location = 0) in vec3 model_position;

uniform mat4 matrix_model_to_hom;

void main()
{
	gl_Position = matrix_model_to_hom * vec4(model_position, 1.0f);
}