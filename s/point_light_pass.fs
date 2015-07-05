//
// point_light_pass.fs
//

#version 330 core

precision mediump float;

#define DIFFUSE_FACTOR 0.8f

uniform float window_width;
uniform float window_height;

uniform vec2 viewport_size;

uniform sampler2D positions_texture;
uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D emissive_texture;
uniform sampler2D specular_texture;

uniform vec4 light_position;
uniform vec3 light_intensity;
uniform vec3 light_attenuation;
uniform int  light_type;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculate_screen_position()
{
    return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
}

void init_variables(inout vec3 view_position, inout vec3 diffuse_color, inout vec3 surface_normal)
{
    vec2 screen_position = calculate_screen_position();
    view_position = texture(positions_texture, screen_position).xyz;
    diffuse_color = texture(diffuse_texture, screen_position).xyz;
    surface_normal = texture(normal_texture, screen_position).xyz;
}

out vec3 final_color;

void main()
{
    // vec3 view_position, diffuse, normal;
    // init_variables(view_position, diffuse, normal);

	vec2 screen_position = calculate_screen_position();
	vec3 view_position = texture(positions_texture, screen_position).xyz;
	vec3 diffuse = texture(diffuse_texture, screen_position).xyz;
    vec3 emissive = texture(emissive_texture, screen_position).xyz;
    vec3 specular = texture(specular_texture, screen_position).xyz;
	vec3 normal = texture(normal_texture, screen_position).xyz;

    vec3 light_dir = light_position.xyz - view_position;
    float light_dist = length(light_dir);
    light_dir = normalize(light_dir);
    // or: light_dir /= light_dist; ?
    float attenuation = light_attenuation.x + light_attenuation.y * light_dist + light_attenuation.z * light_dist * light_dist;
    float lambertian = (DIFFUSE_FACTOR / attenuation) * max(dot(light_dir, normal), 0.0f);
    final_color = lambertian * light_intensity * diffuse;
    final_color = vec3(1.0f, 0.0f, 0.0f);
}