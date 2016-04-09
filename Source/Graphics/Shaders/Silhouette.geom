
#version 330 core

#define EPSILON 0.001f

layout(triangles_adjacency) in;
layout(line_strip, max_vertices = 6) out;

in vec3 viewSpaceVertexPosition[];

uniform vec3 lightPosition;
uniform int debugFlag;

void emitLine(int startIndex, int endIndex)
{
	gl_Position = gl_in[startIndex].gl_Position;
	EmitVertex();

	gl_Position = gl_in[endIndex].gl_Position;
	EmitVertex();

	EndPrimitive();
}

vec3 normalOfTriangle(int firstIndex, int secondIndex, int thirdIndex)
{
	return cross(
		viewSpaceVertexPosition[secondIndex] - viewSpaceVertexPosition[firstIndex], 
		viewSpaceVertexPosition[thirdIndex]  - viewSpaceVertexPosition[firstIndex]);
}

void main()
{

	/*
	
	1___________2____e4____3
	\          /\          /
	 \        /  \        /
	  e3    e1    e5     /
	   \    /      \    /
	    \  /        \  /
	     0/____e2____\4
	      \          /
	       \        /
	        e6     /
	         \    /
	          \  /
	           \5

	 */
	
	vec3 e1 = viewSpaceVertexPosition[2] - viewSpaceVertexPosition[0];
	vec3 e2 = viewSpaceVertexPosition[4] - viewSpaceVertexPosition[0];
	vec3 e3 = viewSpaceVertexPosition[1] - viewSpaceVertexPosition[0];
	vec3 e4 = viewSpaceVertexPosition[3] - viewSpaceVertexPosition[2];
	vec3 e5 = viewSpaceVertexPosition[4] - viewSpaceVertexPosition[2];
	vec3 e6 = viewSpaceVertexPosition[5] - viewSpaceVertexPosition[0];

	vec3 lNormal = cross(e1, e2);
	vec3 lDirectionFromVertexToLight = lightPosition - viewSpaceVertexPosition[0];

	if (dot(lNormal, lDirectionFromVertexToLight) > 0.00001f)
	{
		lNormal = cross(e3, e1);

		if (dot(lNormal, lDirectionFromVertexToLight) <= 0.0f) emitLine(0, 2);

		lNormal = cross(e4, e5);
		lDirectionFromVertexToLight = lightPosition - viewSpaceVertexPosition[2];

		if (dot(lNormal, lDirectionFromVertexToLight) <= 0.0f) emitLine(2, 4);

		lNormal = cross(e2, e6);
		lDirectionFromVertexToLight = lightPosition - viewSpaceVertexPosition[4];

		if (dot(lNormal, lDirectionFromVertexToLight) <= 0.0f) emitLine(4, 0);
	}
}