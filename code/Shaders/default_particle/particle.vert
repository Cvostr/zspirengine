#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec2 UVCoord; 

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
	uniform vec3 cam_position;
};

layout (std140, binding = 9) uniform InstMatrices{
    uniform mat4 inst_transform[1000];
};

layout (std140, binding = 11) uniform Time{
    uniform uint Frames;
	uniform float DeltaTime;
};

void main(){
	UVCoord = uv;

	FragPos = (inst_transform[gl_InstanceID] * vec4(position, 1.0)).xyz;
    //FragPos = (object_transform * vec4(position, 1.0)).xyz;
    gl_Position = cam_projection * cam_view * vec4(FragPos, 1);
	
	//gl_Position = cam_projection * cam_view * object_transform * vec4(position, 1.0);
	
}
 
