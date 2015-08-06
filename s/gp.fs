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

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

layout (location = GBUFFER_POSITION) out vec3 out_position;

#ifdef MATERIAL_DIFFUSE_TEXTURE || MATERIAL_DIFFUSE_COLOR
	layout (location = GBUFFER_DIFFUSE) out vec4 out_diffuse;
#endif

#ifdef MATERIAL_SPECULAR_TEXTURE || MATERIAL_SPECULAR_COLOR
	layout (location = GBUFFER_SPECULAR) out vec4 out_specular;
#endif

layout (location = GBUFFER_NORMAL) out vec3 out_normal;

uniform struct Material 
{
	#ifdef MATERIAL_DIFFUSE_COLOR
		vec4 diffuse_color;
	#endif

	#ifdef MATERIAL_SPECULAR_COLOR
		vec4 specular_color;
	#endif

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

	#if defined(MATERIAL_DIFFUSE_COLOR) && defined(MATERIAL_DIFFUSE_TEXTURE)
		out_diffuse = material.diffuse_color * texture(material.diffuse_texture, v_texcoord);
	#elif defined(MATERIAL_DIFFUSE_COLOR)
		out_diffuse = material.diffuse_color;
	#elif defined(MATERIAL_DIFFUSE_TEXTURE)
		out_diffuse = texture(material.diffuse_texture, v_texcoord);
	#endif

	if defined(MATERIAL_SPECULAR_COLOR) && defined(MATERIAL_DIFFUSE_TEXTURE)
		out_specular = material.specular_color * texture(material.specular_texture, v_texcoord);
	#elif defined(MATERIAL_SPECULAR_COLOR)
		out_specular = material.specular_color;
	#elif defined(MATERIAL_SPECULAR_TEXTURE)
		out_specular = texture(material.specular_texture, v_texcoord);
	#endif

	#if defined(MATERIAL_NORMAL_TEXTURE)

	#else
		out_normal = normalize(v_normal);
	#endif
}
