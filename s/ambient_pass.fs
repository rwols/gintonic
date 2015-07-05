//
// ambient_pass.fs
//

#version 330 core

precision mediump float;

#define AMBIENT_FACTOR 0.2f

uniform vec2 viewport_size;
uniform sampler2D diffuse_texture;
uniform sampler2D debug_texture;

// Returns the current fragment's clip-space coordinates, for
// fetching it from the g-buffer.
vec2 calculate_screen_position()
{
	return vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);
}

out vec3 final_color;

void main()
{
	vec2 screen_position = calculate_screen_position();
	final_color = AMBIENT_FACTOR * texture(diffuse_texture, screen_position).xyz;
	final_color += texture(debug_texture, screen_position).xyz;

	// debug
	//final_color = vec3(screen_position, 0.0f);
}