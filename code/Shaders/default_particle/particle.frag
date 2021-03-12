#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 3) out vec4 tSpec;
layout (location = 4) out vec4 tMasks;

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec2 UVCoord;
//textures
layout(binding = 0) uniform sampler2D diffuse;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

void main(){

	vec2 uv = UVCoord;
	
	vec4 diffuse = texture(diffuse, uv);
	
    FragColor = diffuse;    
    tPos = FragPos;
    tMasks.r = 1;
}
 
