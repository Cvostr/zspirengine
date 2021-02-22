#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

layout (location = 0) out vec4 ClipSpace;
layout (location = 1) out vec2 DistortionUV;
layout (location = 2) out vec2 UV;
layout (location = 3) out vec3 FragPos;
layout (location = 4) out vec3 OrigNormal;
layout (location = 5) out mat3 TBN;

void main(){
    vec4 FragPos4 = object_transform * vec4(position, 1.0);
	gl_Position = cam_projection * cam_view * FragPos4;
    
	ClipSpace = gl_Position;
    UV = uv;
    FragPos = FragPos4.xyz;
    OrigNormal = normal;

    DistortionUV = vec2(position.x, position.y) / 2 + 0.5;

    vec3 TangentVec = normalize(vec3(object_transform * vec4(tangent, 0)));
	vec3 BiTangentVec = normalize(vec3(object_transform * vec4(bitangent, 0)));
	vec3 NormalVec = normalize(vec3(object_transform * vec4(normal, 0)));
	TBN = transpose(mat3(TangentVec, BiTangentVec, NormalVec));
}
