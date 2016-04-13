
#version 330 core

#define DIR_EPSILON 0.00001f
#define TEST_EPSILON 0.0f

#define getVertex(index) gl_in[index].gl_Position

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 18) out;

uniform vec3 lightPosition; // in LOCAL coordinates
uniform mat4 matrixPVM;

vec4 normalOfTriangle(in int firstIndex, in int secondIndex, in int thirdIndex)
{
	return vec4
	(
		cross
		(
			getVertex(secondIndex).xyz - getVertex(firstIndex).xyz, 
			getVertex(thirdIndex).xyz  - getVertex(firstIndex).xyz
		), 
		0.0f
	);
}

void emitQuadToInfinity(in vec4 beginVertex, in vec4 endVertex)
{
	vec4 lLightPos4 = vec4(lightPosition, 1.0f);
	vec4 lDirection = beginVertex - lLightPos4;

	gl_Position = matrixPVM * beginVertex;
	EmitVertex();
	gl_Position = matrixPVM * (beginVertex - lLightPos4);
	EmitVertex();
	gl_Position = matrixPVM * endVertex;
	EmitVertex();
	gl_Position = matrixPVM * (endVertex - lLightPos4);
	EmitVertex();
	EndPrimitive();
}

void emitQuadToInfinity(in int beginIndex, in int endIndex)
{
	emitQuadToInfinity(getVertex(beginIndex), getVertex(endIndex));
}

void emitCaps()
{
	vec4 lLightPos4 = vec4(lightPosition, 1.0f);
	vec4 lDirection = getVertex(0) - lLightPos4;

	// FRONT CAP: Just a tiny bit *away* from the original triangle.
	gl_Position = matrixPVM * (getVertex(0) + lDirection * DIR_EPSILON);
	EmitVertex();
	lDirection = getVertex(2) - lLightPos4;
	gl_Position = matrixPVM * (getVertex(2) + lDirection * DIR_EPSILON);
	EmitVertex();
	lDirection = getVertex(4) - lLightPos4;
	gl_Position = matrixPVM * (getVertex(4) + lDirection * DIR_EPSILON);
	EmitVertex();
	EndPrimitive();

	// BACK CAP: at "infinity".
	gl_Position = matrixPVM * (getVertex(0) - lLightPos4);
	EmitVertex();
	gl_Position = matrixPVM * (getVertex(4) - lLightPos4);
	EmitVertex();
	gl_Position = matrixPVM * (getVertex(2) - lLightPos4);
	EmitVertex();
	EndPrimitive();

}

void main()
{

	/*
	
	1___________2__________3
	\          /\          /
	 \        /  \        /
	  \      /    \      /    CLOCKWISE winding order (I think)
	   \    /      \    /
	    \  /        \  /
	     0/__________\4
	      \          /
	       \        /
	        \      /
	         \    /
	          \  /
	           \5

	 */

	vec4 lLightPos4 = vec4(lightPosition, 1.0f);
	vec4 lNormal = normalOfTriangle(0, 2, 4);
	vec4 lDirection = lLightPos4 - getVertex(0);

	// This is true if and only if the triangle faces the light.
	if (dot(lNormal, lDirection) > TEST_EPSILON)
	{
		lNormal = normalOfTriangle(0, 1, 2);

		// The neighboring triangle [0, 1, 2] does NOT
		// face the light if and only if this is true.
		if (dot(lNormal, lDirection) <= 0.0f)
		{
			// It does not face the light,
			// so [0, 2] is a silhouette edge.
			emitQuadToInfinity(0, 2);
		}

		lNormal = normalOfTriangle(2, 3, 4);
		lDirection = lLightPos4 - getVertex(2);

		// The neighboring triangle [2, 3, 4] does NOT
		// face the light if and only if this is true.
		if (dot(lNormal, lDirection) <= 0.0f)
		{
			// It does not face the light,
			// so [2, 4] is a silhouette edge.
			emitQuadToInfinity(2, 4);
		}

		lNormal = normalOfTriangle(4, 5, 0);
		lDirection = lLightPos4 - getVertex(4);

		// The neighboring triangle [4, 5, 0] does NOT
		// face the light if and only if this is true.
		if (dot(lNormal, lDirection) <= 0.0f)
		{
			// It does not face the light,
			// so [4, 0] is a silhouette edge.
			emitQuadToInfinity(4, 0);
		}

		emitCaps();
	}
}