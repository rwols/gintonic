//
// gp.fs
//
// This is an attempt at a generic geometry pass shader
// So far this isn't working.
//

#version 330

#define GBUFFER_POSITION 0
#define GBUFFER_DIFFUSE 1
#define GBUFFER_SPECULAR 2
#define GBUFFER_NORMAL 3
#define GBUFFER_COUNT 4

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

layout (location = GBUFFER_POSITION) out vec3 out_position;
layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse;
layout (location = GBUFFER_SPECULAR) out vec4 out_specular;
layout (location = GBUFFER_NORMAL) out vec3 out_normal;

uniform struct Material 
{
	vec4 diffuse_color;
	vec4 specular_color;

	#ifdef MATERIAL_DIFFUSE_TEXTURE
		sampler2D diffuse_texture;
	#endif

	#ifdef MATERIAL_SPECULAR_TEXTURE
		sampler2D specular_texture;
	#endif

	#ifdef MATERIAL_NORMAL_TEXTURE
		sampler2D normal_texture;
	#endif

} material;

void main()
{
	out_position = v_position;

	out_diffuse = material.diffuse_color;

	#ifdef MATERIAL_DIFFUSE_TEXTURE
		out_diffuse *= texture(material.diffuse_texture, v_texcoord);
	#endif

	out_specular = material.specular_color;

	#ifdef MATERIAL_SPECULAR_TEXTURE
		out_specular *= texture(material.specular_texture, v_texcoord);
	#endif

	#ifdef MATERIAL_NORMAL_TEXTURE
		#error not yet implemented, sorry
	#else
		out_normal = normalize(v_normal);
	#endif
}
