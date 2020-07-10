#version 420 core

layout (location = 0) in vec3 position;
//IDs of bones, that control this vertex
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs1;
layout (location = 7) in ivec4 BoneIDs2;
//Coefficient of bones, that control this vertex
layout (location = 8) in vec4 Weights;
layout (location = 9) in vec4 Weights1;
layout (location = 10) in vec4 Weights2;
//Amount of bones, that control this vertex
layout (location = 11) in int bones;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

layout (std140, binding = 4) uniform BonesData{
    uniform mat4 bone_transform[200];
};

layout (std140, binding = 9) uniform InstMatrices{
    uniform mat4 inst_transform[1000];
};


void main(){
	gl_Position =  cam_projection * cam_view * (inst_transform[gl_InstanceID]) * vec4(position, 1.0);
}
