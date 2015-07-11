//
// pn_directional.fs
//
// forward lighting with incoming position and normal
//

#version 330 core

in vec3 v_position;
in vec3 v_normal;

uniform vec3 color;

uniform struct Light
{
	vec3 direction;
	vec3 intensity;
} light;

out vec4 final_color;

void main()
{
	vec4 diffuse = vec4(color, 1);

	vec3 light_dir = normalize(v_position.xyz - light.direction);

	vec3 light_contribution = max(dot(light_dir, v_normal), 0.0f) * light.intensity;

    // float attenuation = light.attenuation.x 
    // 	+ light.attenuation.y * light_dist 
    // 	+ light.attenuation.z * light_dist * light_dist;
    // float lambertian = (1.0f / attenuation) * max(dot(light_dir, v_normal), 0.0f);

	final_color = 0.9f * diffuse * vec4(light_contribution, 1.0f) + 0.1f * diffuse;
}