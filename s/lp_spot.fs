//
// lp_spot.fs
//
// Spot lighting
//

#version 330 core

uniform vec2 viewport_size;

uniform vec3 eye_position;

uniform struct GeometryBuffer
{
	sampler2D position; // these are in VIEW coordinates
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
} gbuffer;

uniform struct SpotLight {
	vec4 intensity;
	vec3 position; // in VIEW coordinates
	vec3 direction; // in VIEW coordinates
	vec4 attenuation;
} light;

out vec3 final_color;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculate_screen_position()
{
	return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
}

void main()
{
	vec2 screen_uv = calculate_screen_position();

	vec3 P = texture(gbuffer.position, screen_uv).xyz;
	vec4 diffuse = texture(gbuffer.diffuse, screen_uv);
	vec4 specular = texture(gbuffer.specular, screen_uv);
	vec3 N = texture(gbuffer.normal, screen_uv).xyz;

	// L is the direction from the surface position to the light position
	// P is in VM-coordinates, so light.position must be supplied in
	// VM-coordinates too.
	vec3 L = light.position - P;

	// d is the distance from the surface position to the light position
	float d = length(L);
	
	// Don't forget to normalize L.
	L = normalize(L);

	// E is the direction from the surface to the eye/camera.
	// Since we are in VIEW coordinates, the position of the eye is 0.
	// So the direction from the surface to the eye is 0 - P = - P.
	vec3 E = normalize(-P);

	// R is the reflection direction. When E gets closer to R, specularity enhances.
	vec3 R = normalize(-reflect(L,N));

	// The attenuation factor for a spot light
	float att = 1.0f / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);
	
	// dc is the diffuse contribution.
	float dc = light.intensity.a * diffuse.a * pow(max(dot(L, -light.direction), 0.0f), light.attenuation.w) * att;

	float spec_factor = clamp(pow(max(dot(R,E), 0.0f), specular.a), 0.0f, 1.0f);
	spec_factor *= att;

	final_color = dc * light.intensity.rgb * diffuse.rgb 
		+ spec_factor * specular.rgb * light.intensity.rgb;

	final_color.r += 0.2f;
}