#version 420 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout(location = 0) out vec2 _UV;

void main(){

_UV = uv;

gl_Position = vec4(pos, 1);


}
