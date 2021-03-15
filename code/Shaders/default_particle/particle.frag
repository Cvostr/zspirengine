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

layout (std140, binding = 3) uniform Particle{
    vec3 tint_color;
    bool hasParticleTexture;
};

void main(){

	vec2 uv = UVCoord;
	
	vec4 sprite = texture(diffuse, uv);
    vec3 color = sprite.rgb * tint_color;

    if(sprite.a < 0.05)
        discard;
	
    FragColor = vec4(color, sprite.a);    
    tPos = FragPos;
    tMasks.r = 1;
}
 
