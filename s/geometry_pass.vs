//
// geometry_pass.vs
//

#version 330 core

layout(location = 0) in vec3 model_position;
layout(location = 1) in vec2 model_uv_;
layout(location = 2) in vec3 model_normal;

uniform mat4 matrix_model_to_hom;
uniform mat4 matrix_model_to_view;
uniform mat3 matrix_normal_view;

out vec3 view_position; // the view-space position vector
out vec2 model_uv; // the model-space UV coordinates
out vec3 view_normal; // the view-space normal vector

void main()
{
	gl_Position   = matrix_model_to_hom * vec4(model_position, 1.0f);
	view_position = (matrix_model_to_view * vec4(model_position, 1.0f)).xyz;
	model_uv      = model_uv_;
	view_normal   = matrix_normal_view * model_normal;
}