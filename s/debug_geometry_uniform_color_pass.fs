//
// debug_geometry_uniform_color_pass.fs
//

#version 330 core

#define POSITIONS 0
#define DIFFUSE 1
#define NORMALS 2
#define EMISSIVE 3
#define SPECULAR 4
#define DEBUG 5

uniform vec4 color;

layout (location = DEBUG) out vec3 out_debug;

void main()
{
	out_debug  = color.xyz;
}
