#version 430 core

const int MAX_SPLITS = 4;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 2) uniform ShadowData{
//Shadowmapping stuff
    uniform float ShadowBias; //4
    uniform int ShadowmapSize; //4
    uniform bool HasShadowMap; //4
    //16
    uniform mat4 LightProjViewMat0; // 16 * 4
    uniform mat4 LightProjViewMat1; // 16 * 4
    uniform mat4 LightProjViewMat2; // 16 * 4
    uniform mat4 LightProjViewMat3; // 16 * 4
    uniform mat4 LightProjViewMat4; // 16 * 4
    //336
    uniform int CasterDistance0; //4
    uniform int CasterDistance1; //4
    uniform int CasterDistance2; //4
    uniform int CasterDistance3; //4
    uniform int CasterDistance4; //4
};

in VS_OUTPUT
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