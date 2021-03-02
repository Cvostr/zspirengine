#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 UVCoord;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;


void main() {
    vec3 FragPos = vec4(position, 1).xyz;
    gl_Position = vec4(FragPos, 1.0);
    UVCoord = uv;
}