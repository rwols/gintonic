//
// pn.fs
//
// forward lighting with incoming position and normal
//

#version 330 core

in vec3 v_position;
in vec3 v_normal;

uniform vec3 color;

uniform struct Light
{
	vec3 position;
	vec3 intensity;
	vec3 attenuation;
} light;

out vec4 final_color;

void main()
{
	vec4 diffuse = vec4(color, 1);

	vec3 light_dir = light.position - v_position.xyz;
    float light_dist = length(light_dir);
    light_dir = normalize(light_dir);

    float attenuation = light.attenuation.x 
    	+ light.attenuation.y * light_dist 
    	+ light.attenuation.z * light_dist * light_dist;
    float lambertian = (1.0f / attenuation) * max(dot(light_dir, v_normal), 0.0f);

	final_color = lambertian * attenuation * vec4(light.intensity, 1.0f) * diffuse;
}