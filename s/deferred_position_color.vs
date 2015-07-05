//
// deferred_position_color.vs
//

#version 330 core

layout(location = 0) in vec3 model_position;
layout(location = 1) in vec4 model_color;

uniform mat4 matrix_model_to_hom;
uniform mat4 matrix_model_to_view;

out vec4 view_position;
out vec4 color;

void main()
{
	gl_Position = matrix_model_to_hom * vec4(model_position, 1.0f);
	view_position = (matrix_model_to_view * vec4(model_position, 1.0f)).xyz;
	color = model_color;
}