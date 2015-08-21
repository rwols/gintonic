//
// skybox.vs
//
// Used for drawing a skybox.
// Used by: skybox.fs

#version 330

layout(location = 0) in vec3 in_position;

uniform mat4 matrix_PV;

out vec3 v_texcoord;

void main()
{
	vec4 P = matrix_PV * vec4(in_position, 0.0f);
	gl_Position = P.xyzz;
	v_texcoord = in_position.xyz;
}