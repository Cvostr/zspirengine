#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

layout (location = 0) out vec2 UV;

void main(){

    UV = uv;
	gl_Position =  cam_projection * cam_view * object_transform  * vec4(position, 1.0);
	
}
