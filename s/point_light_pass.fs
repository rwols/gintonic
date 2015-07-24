//
// point_light_pass.fs
//

#version 330

precision mediump float;

#define DIFFUSE_FACTOR 0.8f

uniform vec2 viewport_size;

uniform struct GeometryBuffer {
    sampler2D position;
    sampler2D diffuse;
    sampler2D normal;
} gbuffer;

uniform struct PointLight {
    vec3 intensity;
    vec3 position;
    vec3 attenuation;
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
    // vec3 view_position, diffuse, normal;
    // init_variables(view_position, diffuse, normal);

	vec2 screen_position = calculate_screen_position();
	vec3 view_position = texture(gbuffer.position, screen_position).xyz;
	vec4 diffuse = texture(gbuffer.diffuse, screen_position);
	vec3 normal = texture(gbuffer.normal, screen_position).xyz;

    vec3 light_dir = light.position - view_position;
    float light_dist = length(light_dir);
    light_dir = normalize(light_dir);
    // or: light_dir /= light_dist; ?
    float attenuation = light.attenuation.x + light.attenuation.y * light_dist + light.attenuation.z * light_dist * light_dist;
    float lambertian = (diffuse.a / attenuation) * max(dot(light_dir, normal), 0.0f);
    final_color = lambertian * light.intensity * diffuse.rgb;
}