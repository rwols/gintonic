//
// directional_diffuse.fs
//

#version 330 core

in vec2 v_uv;
in vec3 v_normal;

uniform struct Material
{
	sampler2D diffuse_color;
	float diffuse_factor;
} material;

uniform struct Light
{
	vec3 direction;
	vec3 intensity;
} light;

out vec4 final_color;

void main()
{
	float ambient_factor = 1.0f - material.diffuse_factor;
	vec4 diffuse = texture(material.diffuse_color, v_uv);
	float light_factor = max(dot(-light.direction, v_normal), 0.0f);
	float lambertian = material.diffuse_factor * light_factor;
	final_color = lambertian * vec4(light.intensity, 1.0f) * diffuse;
	final_color += ambient_factor * diffuse;
}