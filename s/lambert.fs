//
// lambert.fs
//

#version 330 core

precision mediump float;

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_UNKNOWN -1

uniform float diffuse_factor;
uniform float specular_shininess;
uniform sampler2D diffuse_texture;

// light_position must be supplied in view-coordinates.
// so, multiply the world-coordinates of the light's position
// with the world-to-view matrix transformation on the cpu.
uniform vec4 light_position;
uniform vec3 light_intensity;
uniform vec3 light_attenuation;

in vec3 view_position;
in vec2 model_uv;
in vec3 view_normal;

out vec3 final_color;

int light_type()
{
    if (light_position.w == 1.0f) return LIGHT_TYPE_POINT;
    else if (light_position.w == 0.0f) return LIGHT_TYPE_DIRECTIONAL;
    else return LIGHT_TYPE_UNKNOWN;
}

void main()
{
    vec4 the_diffuse_texture = texture(diffuse_texture, model_uv);
    vec3 normal = normalize(view_normal);
    vec3 view_dir = normalize(-view_position);
    int the_light_type = light_type();

    if (the_light_type == LIGHT_TYPE_DIRECTIONAL)
    {
        vec3 light_dir = normalize(-light_position.xyz);
        float lambertian = (1.0f - diffuse_factor) + diffuse_factor * max(dot(light_dir, normal), 0.0f);
        final_color = lambertian * light_intensity * the_diffuse_texture.rgb;
    }
    else if (the_light_type == LIGHT_TYPE_POINT)
    {
        vec3 light_dir = light_position.xyz - view_position;
        float light_dist = length(light_dir);
        light_dir = normalize(light_dir);
        // or: light_dir /= light_dist; ?
        float attenuation = light_attenuation.x + light_attenuation.y * light_dist + light_attenuation.z * light_dist * light_dist;
        float lambertian = ((1.0f - diffuse_factor) + diffuse_factor / attenuation) * max(dot(light_dir, normal), 0.0f);
        final_color = lambertian * light_intensity * the_diffuse_texture.rgb;
    }
}