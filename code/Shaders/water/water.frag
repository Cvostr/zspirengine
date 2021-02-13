#version 450 core

layout(location = 0) in vec4 ClipSpace;
layout(location = 1) in vec2 DistortionUV;

layout(location = 0) out vec4 FragColor;


layout(binding = 0) uniform sampler2D reflectionMap;
layout(binding = 1) uniform sampler2D distortionMap;
layout(binding = 2) uniform sampler2D normalMap;

layout (std140, binding = 51) uniform WaterData{
    bool hasReflectionMap; //0
    bool hasDistortionMap; //4
    bool hasNormalMap; // 8
    float DistrtionFactor; //12
    vec4 DiffuseColor; //16
};

void main(){

    vec4 color = vec4(0, 0, 0, 0);

    vec2 ReflectionUV = (ClipSpace.xy / ClipSpace.z) / 2 + 0.5;
    ReflectionUV.y *= -1;

    vec2 Distortion = vec2(0, 0);
    if(hasDistortionMap)
        Distortion = texture(distortionMap, DistortionUV).rg * 2.0 - 1.0;

    vec4 Reflection = texture(reflectionMap, ReflectionUV + Distortion);
    

    color = Reflection;
    color *= DiffuseColor;
    color.a = 0.67;

    FragColor = color;
}
