#version 450 core

//Output attachments
layout(location = 0) out vec4 tDiffuse;
layout(location = 1) out vec3 tNormal;
layout(location = 2) out vec3 tPos;
layout (location = 3) out vec4 tSpec;
layout (location = 4) out vec4 tMasks;

// From vertex shader
layout(location = 0) in vec4 ClipSpace;
layout(location = 1) in vec2 DistortionUV;
layout(location = 2) in vec2 UV;
layout(location = 3) in vec3 FragPos;
layout(location = 4) in vec3 OrigNormal;
layout(location = 5) in mat3 TBN;

//Textures
layout(binding = 0) uniform sampler2D reflectionMap;
layout(binding = 1) uniform sampler2D distortionMap;
layout(binding = 2) uniform sampler2D normalMap;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

layout (std140, binding = 3) uniform WaterData{
    bool hasReflectionMap; //0
    bool hasDistortionMap; //4
    bool hasNormalMap; // 8
    float DistrtionFactor; //12
    vec4 DiffuseColor; //16
};

void main(){

    vec4 color = vec4(0, 0, 0, 0);
    vec3 Normal = vec3(0, 0, 0);

    vec2 ReflectionUV = (ClipSpace.xy / ClipSpace.z) / 2 + 0.5;
    ReflectionUV.y *= -1;

    vec2 Distortion = vec2(0, 0);
    if(hasDistortionMap)
        Distortion = texture(distortionMap, DistortionUV).rg * 2.0 - 1.0;

    vec4 Reflection = texture(reflectionMap, ReflectionUV + Distortion);
    
    if(hasNormalMap){
        Normal = texture(normalMap, UV).xyz;
		Normal = normalize(Normal * 2 - 1);
		Normal = normalize(TBN * Normal);
	}

    vec3 CamToWater = normalize(cam_position - FragPos);
    float angle = max(dot(CamToWater, vec3(0,1,0)), 0.0);

    color = Reflection;
    color += DiffuseColor;
    color.a = (1 - angle);

    tDiffuse = color;
    tNormal = Normal;
    tPos = FragPos;
    tMasks = vec4(1.0, 0, 0, 0);
    tSpec.r = 5.f / 255.f;
}
