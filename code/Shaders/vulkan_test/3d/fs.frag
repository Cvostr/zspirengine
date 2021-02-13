#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 _uv;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPos;
layout(location = 3) out vec4 tSpec;
layout(location = 4) out vec4 tMasks;

layout(set = 1, binding = 0) uniform sampler2D diffuse;
layout(set = 1, binding = 1) uniform sampler2D normal_map;
layout(set = 1, binding = 2) uniform sampler2D specular_map;
layout(set = 1, binding = 3) uniform sampler2D height_map;
layout(set = 1, binding = 4) uniform sampler2D occlusion_map;
layout(set = 1, binding = 27) uniform sampler2DArray shadow_map;

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

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    //Camera position
    uniform vec3 cam_position;
};

layout( push_constant ) uniform material_constants
{
	layout(offset = 64) vec3 diffuse_color;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    bool hasHeightMap;
    bool hasAoMap;

    float material_shininess;
    
    int _u;
    int _v;
} PushConstants;

vec2 processParallaxMapUv(vec2 uv){
    if(!PushConstants.hasHeightMap) return uv;
    
    float height = texture(height_map, uv).r;
    vec3 camera_dir = normalize(TBN * FragPos - TBN * cam_position);
    
    vec2 uv_offset = camera_dir.xy / camera_dir.z * (height);
    return uv - uv_offset;
}

void main() {
    vec3 result = vec3(1.0, 1.0, 1.0); //Default value
    vec3 Normal = InNormal;
    float result_shininess = PushConstants.material_shininess;

    vec2 uv = vec2(_uv.x * PushConstants._u, _uv.y * PushConstants._v);

    if(PushConstants.hasDiffuseMap)
	    result = texture(diffuse, processParallaxMapUv(uv)).rgb; 
    
    if(PushConstants.hasNormalMap){
        Normal = texture(normal_map, uv).rgb;
        Normal = normalize(Normal * 2 - 1);
		Normal = normalize(TBN * Normal);
    }

    if(PushConstants.hasSpecularMap)
        result_shininess *= texture(specular_map, uv).r;

    outColor = vec4(result, 1.0);
    outNormal = vec4(Normal, 1);
    outPos = vec4(FragPos, 1);
    tSpec.r = result_shininess / 255.f;
    tMasks = vec4(1.0, 0, 0, 0);
}