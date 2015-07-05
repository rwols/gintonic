//
// geometry_pass.fs
//

#version 330 core

#define POSITIONS 0
#define DIFFUSE 1
#define NORMALS 2
#define EMISSIVE 3
#define SPECULAR 4
#define DEBUG 5

#define TYPE_LAMBERT 0
#define TYPE_PHONG 1

in vec3 view_position;
in vec2 model_uv;
in vec3 view_normal;

layout (location = POSITIONS) out vec3 out_position;
layout (location = DIFFUSE)   out vec3 out_diffuse;
layout (location = NORMALS)   out vec3 out_normal;
layout (location = EMISSIVE)  out vec3 out_emissive;
layout (location = SPECULAR)  out vec3 out_specular;

uniform int type;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_emissive;
uniform sampler2D texture_specular;
uniform float shininess;
uniform float reflectivity;
uniform float transparency;

uniform bool object_emits_light;

void main()
{
	// TODO: make this so that
	// the normal texture actually influences
	// the normal vector
	vec3 tex_normal = texture(texture_normal, model_uv).xyz;

	// these are here so that the compiler
	// does not remove (optimize out) these uniforms,
	// so that we do not get thrown an
	// exception in our face.
	float temp_shininess = shininess + 0.1f;
	float temp_reflectivity = reflectivity + 0.1f;
	float temp_transparency = transparency + 0.1f;

	out_position = view_position;
	out_diffuse  = texture(texture_diffuse, model_uv).xyz;
	out_normal   = normalize(view_normal);
	out_specular = texture(texture_specular, model_uv).xyz;

	if (object_emits_light) out_emissive = texture(texture_emissive, model_uv).xyz;
}
