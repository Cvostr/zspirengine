#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 fragColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

void main() {
    gl_Position = vec4(cam_position, 1.0);
    fragColor = colors[gl_VertexIndex];
}