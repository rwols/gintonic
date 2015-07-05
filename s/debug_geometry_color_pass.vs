//
// debug_geometry_color_pass.vs
//

#version 330 core

layout(location = 0) in vec3 model_position;
layout(location = 1) in vec4 model_color_;

uniform mat4 matrix_model_to_hom;

out vec3 model_color;

void main()
{
	gl_Position = matrix_model_to_hom * vec4(model_position, 1.0f);
	model_color = model_color_.xyz;
}