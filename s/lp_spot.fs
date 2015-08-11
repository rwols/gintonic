//
// lp_point.fs
//
// Point lighting
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

uniform struct PointLight {
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

	float maxdotNL = max(dot(N,L), 0.0f);

	// The attenuation factor for a point light
	float att = 1.0f / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);
	
	// dc is the diffuse contribution.
	float dc = light.intensity.a * diffuse.a * maxdotNL * att;

	float spotfactor = clamp(pow(max(dot(-light.direction, L), 0.0f), light.attenuation.w), 0.0f, 1.0f);

	dc *= spotfactor;

	float spec_factor = 0.0f;
	if (maxdotNL > 0.0f)
	{
		vec3 E = normalize(-P);
		vec3 R = normalize(-reflect(L,N));
		spec_factor = clamp(pow(max(dot(R,E), 0.0f), specular.a), 0.0f, 1.0f);
	}

	final_color = dc * light.intensity.rgb * diffuse.rgb 
		+  (spec_factor * specular.rgb * light.intensity.rgb);

	// Debug the sphere stencil pass
	// final_color.r += 0.1f;
}