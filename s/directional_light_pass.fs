//
// directional_light_pass.fs
//

#version 330 core

#define DIFFUSE_FACTOR 0.9f

uniform vec2 viewport_size;

uniform struct GeometryBuffer
{
	sampler2D position;
	sampler2D diffuse;
	sampler2D normal;
} gbuffer;

uniform struct DirectionalLight {
	vec3 intensity;
	vec3 direction;
} light;

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
	// vec3 view_position = texture(gbuffer.position, screen_position).xyz;
	vec4 diffuse = texture(gbuffer.diffuse, screen_position);
	// float diffuse_factor = texture(gbuffer.diffuse, screen_position).w;
	vec3 normal = texture(gbuffer.normal, screen_position).xyz;

	vec3 light_dir = normalize(-light.direction);

	float lambertian = max(dot(light_dir, normal), 0.0f);

	float diffuse_factor = diffuse.a;
	float ambient_factor = 1.0f - diffuse_factor;

	final_color = diffuse_factor * lambertian * light.intensity * diffuse.rgb + ambient_factor * diffuse.rgb;
}