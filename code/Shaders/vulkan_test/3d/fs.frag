#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 _uv;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPos;

layout(set = 1, binding = 0) uniform sampler2D diffuse;
layout(set = 1, binding = 1) uniform sampler2D normal;
layout(set = 1, binding = 2) uniform sampler2D spec;
layout(set = 1, binding = 3) uniform sampler2D height;
layout(set = 1, binding = 4) uniform sampler2D ambient;

layout (std140, binding = 2) uniform ShadowData{
//Shadowmapping stuff
    uniform float ShadowBias; //4
    uniform int ShadowmapSize; //4
    uniform bool HasShadowMap; //4
    uniform int CascadesNum; //4
    uniform int PcfPassNum; // 4
    uniform float ShadowStrength; // 4
    //32
    uniform mat4 LightProjViewMat0; // 16 * 4
    uniform mat4 LightProjViewMat1; // 16 * 4
    uniform mat4 LightProjViewMat2; // 16 * 4
    uniform mat4 LightProjViewMat3; // 16 * 4
    uniform mat4 LightProjViewMat4; // 16 * 4
    uniform mat4 LightProjViewMat5; // 16 * 4
    //416   
    uniform int CasterDistance0; //4
    uniform int CasterDistance1; //4
    uniform int CasterDistance2; //4
    uniform int CasterDistance3; //4
    uniform int CasterDistance4; //4
    uniform int CasterDistance5; //4
};

layout( push_constant ) uniform material_constants
{
	layout(offset = 64)vec3 diffuse_color;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    bool hasHeightMap;
    bool hasAoMap;

    float material_shininess;
    
    int _u;
    int _v;
} PushConstants;

void main() {
    vec3 result = vec3(1.0, 1.0, 1.0); //Default value
    vec3 Normal = InNormal;
    if(PushConstants.hasDiffuseMap)
	    result = texture(diffuse, _uv).rgb; 
    
    if(PushConstants.hasNormalMap){
        Normal = texture(normal, _uv).rgb;
        Normal = normalize(Normal * 2 - 1);
		Normal = normalize(TBN * Normal);
    }

    outColor = vec4(result, 1.0);
    outNormal = vec4(Normal, 1);
    outPos = vec4(FragPos, 1);
}