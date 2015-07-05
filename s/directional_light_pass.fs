//
// directional_light_pass.fs
//

#version 330 core

precision mediump float;

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_UNKNOWN -1

#define DIFFUSE_FACTOR 0.9f

uniform float window_width;
uniform float window_height;

uniform vec2 viewport_size;

uniform sampler2D positions_texture;
uniform sampler2D diffuse_texture;
uniform sampler2D normals_texture;
uniform sampler2D specular_texture;
uniform sampler2D emissive_texture;

uniform vec4 light_direction; // in VIEW coordinates
uniform vec3 light_intensity;
uniform int  light_type;

uniform bool object_emits_light;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculate_screen_position()
{
	return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
}

out vec3 final_color;

void main()
{
	vec2 screen_position = calculate_screen_position();
	vec3 view_position = texture(positions_texture, screen_position).xyz;
	vec3 diffuse = texture(diffuse_texture, screen_position).xyz;
	vec3 normal = texture(normals_texture, screen_position).xyz;
	vec3 specular = texture(specular_texture, screen_position).xyz;
	vec3 emissive = texture(emissive_texture, screen_position).xyz;

	vec3 light_dir = normalize(-light_direction.xyz);

	if (light_type == LIGHT_TYPE_DIRECTIONAL)
	{
		float lambertian = DIFFUSE_FACTOR * max(dot(light_dir, normal), 0.0f);
		final_color = lambertian * light_intensity * diffuse;
	}
	else if (light_type == LIGHT_TYPE_POINT)
	{
		// do nothing
	}
	else
	{
		// do nothing
	}

	if (object_emits_light) final_color += DIFFUSE_FACTOR * emissive;

	// final_color = normal;
}