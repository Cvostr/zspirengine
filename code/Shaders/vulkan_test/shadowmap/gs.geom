#version 450 core

const int MAX_SPLITS = 4;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (location = 0) in VS_OUTPUT
{
	float depth;
	flat int instanceID;
} gsinput[];

void main() { 

    for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		gl_Layer = gsinput[i].instanceID;
		EmitVertex();
	}
	EndPrimitive();
} 