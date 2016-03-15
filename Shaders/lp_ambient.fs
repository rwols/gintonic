//
// lp_ambient.fs
//
// Uses lp.vs
//

#version 330 core

uniform vec2 viewport_size;

uniform struct GeometryBuffer
{
	sampler2D diffuse;
} gbuffer;

uniform struct AmbientLight
{
	vec4 intensity;
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
	vec4 diffuse = texture(gbuffer.diffuse, calculate_screen_position());
	final_color = (1.0f - diffuse.a) * light.intensity.a * light.intensity.rgb * diffuse.rgb;
}