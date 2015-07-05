//
// geometry_buffer.fs
//

#version 330 core

#define POSITIONS 0
#define DIFFUSE 1
#define NORMALS 2
#define UVS 3

in vec3 view_position;
in vec2 model_uv;
in vec3 view_normal;

layout (location = POSITIONS) out vec3 out_position;   
layout (location = DIFFUSE)   out vec3 out_diffuse;     
layout (location = NORMALS)   out vec3 out_normal;     
layout (location = UVS)       out vec3 out_uv;

uniform sampler2D texture_diffuse;

void main()
{
	out_position = view_position;
	out_diffuse  = texture(texture_diffuse, model_uv).xyz;
	out_normal   = normalize(view_normal);
	out_uv       = vec3(model_uv, 0.0f);
}
