#version 420 core

#define RENDER_SPRITE 1
#define RENDER_TEXT 2

layout(location = 0) in vec2 _UV;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D scene_map;
layout(binding = 1) uniform sampler2D ui_map;

void main(){

vec4 scene;

	scene = texture(scene_map, _UV);

FragColor = scene;

}
