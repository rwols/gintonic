//
// common.fs
//

#version 330 core

#define AMBIENT_FACTOR 0.1f
#define DIFFUSE_FACTOR 0.9f

struct DirectionalLight
{
	vec3 intensity;
	vec3 direction;
};

struct PointLight
{
	vec3 intensity;
	vec3 position;
	vec3 attenuation;
};