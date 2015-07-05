//
// point_diffuse.fs
//

#version 330 core

in vec4 v_position;
in vec2 v_uv;
in vec3 v_normal;

uniform struct Material
{
	sampler2D diffuse_color;
	float diffuse_factor;
} material;

uniform struct Light
{
	vec3 position;
	vec3 intensity;
	vec3 attenuation;
} light;

out vec4 final_color;

void main()
{
	float ambient_factor = 1.0f - material.diffuse_factor;
	vec4 diffuse = texture(material.diffuse_color, v_uv);

	vec3 light_dir = light.position - v_position.xyz;
    float light_dist = length(light_dir);
    light_dir = normalize(light_dir);

    float attenuation = light.attenuation.x 
    	+ light.attenuation.y * light_dist 
    	+ light.attenuation.z * light_dist * light_dist;
    float lambertian = (material.diffuse_factor / attenuation) * max(dot(light_dir, v_normal), 0.0f);

	final_color = ambient_factor * diffuse;
	final_color += lambertian * attenuation * vec4(light.intensity, 1.0f) * diffuse;
}