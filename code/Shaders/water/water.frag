#version 430 core

layout(location = 0) in vec2 _UV;
layout(location = 0) out vec4 FragColor;


layout(binding = 0) uniform sampler2D reflectionMap;
layout(binding = 1) uniform sampler2D refractionMap;
layout(binding = 2) uniform sampler2D distortionMap;

layout (std140, binding = 51) uniform WaterData{
    bool hasReflectionMap;
    bool hasRefractionMap;
    bool hasDistortionMap;
    float DistrtionFactor;
    vec4 DiffuseColor;
};

void main(){

    vec4 color = vec4(0,0,0,1);

    vec4 Reflection = texture(reflectionMap, _UV);
    vec4 Refraction = texture(refractionMap, _UV);
    vec4 Distortion = texture(distortionMap, _UV);

    color = mix(Reflection, Refraction, 0.3);

    FragColor = color;
}
