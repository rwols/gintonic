//
// lp_spot.fs
//
// Uses lp_pvm.fs
//

#version 330 core

#define M_PI 3.1415926535897932384626433832795f

uniform vec2 viewport_size;

uniform int debugflag;

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
	float angle; // in COSINE of RADIANS
} light;

out vec3 final_color;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculate_screen_position()
{
	return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
}

float maxdot(in vec3 A, in vec3 B)
{
	return max(dot(A,B), 0.0f);
}

float clamppowmaxdot(in vec3 A, in vec3 B, in float exponent)
{
	// return pow(max(dot(A,B), 0.0f), exponent);
	return clamp(pow(maxdot(A,B), exponent), 0.0f, 1.0f);
}

float spotlightfactor(in vec3 A, in vec3 B, in float exponent)
{
	float angle = dot(A, B);
	return (angle >= light.angle) ? clamppowmaxdot(A, B, exponent) : 0.0f;	
}

float quadratic_poly(in float a, in float b, in float c, in float x)
{
	return a + b * x + c * x * x;
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

	float dc = 0.0f;
	float sc = 0.0f;
	float att = 0.0f;

	// Don't forget to normalize L.
	L = normalize(L);

	// The attenuation factor for a point light
	att = 1.0f / quadratic_poly(light.attenuation.x, light.attenuation.y, light.attenuation.z, d);

	// This line is the ONLY difference with respect to a point light...
	// Note also that, if light.attenuation.w == 0.0f, then a spot light
	// effectively becomes a point light.
	att *= spotlightfactor(-light.direction, L, light.attenuation.w);
	
	// dc is the diffuse contribution.
	dc = maxdot(N,L);

	// Since we are in VIEW coordinates, the eye position is at the origin.
	// Therefore the unit direction vector from the point on the surface P
	// to the eye is given by (0 - P) / (||0 - P||) = normalize(-P).
	vec3 E = normalize(-P);

	// We reflect the E direction vector *around* the surface normal.
	// The idea is now that if the angle of incidence of the light is equal
	// to the outgoing angle of incidence to the eye, we experience specularity.
	vec3 R = reflect(E,N);

	// sc is the specular contribution.
	sc = dc > 0.0f ? clamppowmaxdot(R,E, specular.a) : 0.0f;
	
	final_color = light.intensity.a * att * diffuse.a * light.intensity.rgb * (dc * diffuse.rgb + sc * specular.rgb);

}