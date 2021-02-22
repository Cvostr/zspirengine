#version 450

#extension GL_ARB_separate_shader_objects : enable

#include "../HEADER.glsl"

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 InNormal;
layout(location = 2) out vec2 _uv;
layout(location = 3) out mat3 TBN;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
//IDs of bones, that control this vertex
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs1;
layout (location = 7) in ivec4 BoneIDs2;
//Coefficient of bones, that control this vertex
layout (location = 8) in vec4 Weights;
layout (location = 9) in vec4 Weights1;
layout (location = 10) in vec4 Weights2;
//Amount of bones, that control this vertex
layout (location = 11) in uint bones;

layout( push_constant ) uniform constants
{
    int TransformMatIndex;
    int SkinningMatIndex;
    int CameraIndex;
} PushConstants;

layout(std140, set = 2, binding = 0) readonly buffer TransformBuffer{   
	mat4 transforms[];
} transformBuffer;

layout(std140, set = 2, binding = 1) readonly buffer BonesBuffer{   
	mat4 bone_transform[];
} bonesBuffer;

layout(std140, set = 2, binding = 2) readonly buffer CamsBuffer{   
	CameraInfo cameras[];
} camsBuffer;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

mat4 getBoneTransform(){
    int _ids[12];
    float _weights[12];
    
    _ids[0] = BoneIDs.x;
    _ids[1] = BoneIDs.y;
    _ids[2] = BoneIDs.z;
    _ids[3] = BoneIDs.w;
    _ids[4] = BoneIDs1.x;
    _ids[5] = BoneIDs1.y;
    _ids[6] = BoneIDs1.z;
    _ids[7] = BoneIDs1.w;
    _ids[8] = BoneIDs2.x;
    _ids[9] = BoneIDs2.y;
    _ids[10] = BoneIDs2.z;
    _ids[11] = BoneIDs2.w;
    
    _weights[0] = Weights.x;
    _weights[1] = Weights.y;
    _weights[2] = Weights.z;
    _weights[3] = Weights.w;
    _weights[4] = Weights1.x;
    _weights[5] = Weights1.y;
    _weights[6] = Weights1.z;
    _weights[7] = Weights1.w;
    _weights[8] = Weights2.x;
    _weights[9] = Weights2.y;
    _weights[10] = Weights2.z;
    _weights[11] = Weights2.w;
    
    mat4 result = mat4(0.0);
    
    int SMI = PushConstants.SkinningMatIndex;

    for(uint i = 0; i < bones; i++){
        result += bonesBuffer.bone_transform[SMI + _ids[i]] * _weights[i];
    }
	
    return result;
}

void main() {

    mat4 bone_t = mat4(1.0);

	if(bones > 0)
		bone_t = getBoneTransform();

    mat4 Transform = transformBuffer.transforms[PushConstants.TransformMatIndex];

    InNormal = normal;
    vec4 vertpos = (Transform * bone_t * vec4(position, 1));
    FragPos = vertpos.xyz;

    vec3 TangentVec = normalize(vec3(Transform * vec4(tangent, 0)));
	vec3 BiTangentVec = normalize(vec3(Transform * vec4(bitangent, 0)));
	vec3 NormalVec = normalize(vec3(Transform * vec4(normal, 0)));
	TBN = transpose(mat3(TangentVec, BiTangentVec, NormalVec));

    mat4 CamProjection = camsBuffer.cameras[PushConstants.CameraIndex].Projection;
    mat4 CamView = camsBuffer.cameras[PushConstants.CameraIndex].View;

    gl_Position = CamProjection * CamView * vertpos;
    _uv = uv;
}