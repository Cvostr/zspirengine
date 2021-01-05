#version 430 core

layout(location = 0) in vec2 _UV;
layout(location = 0) out vec4 FragColor;

layout (std140, binding = 10) uniform UiText{
    vec3 text_color;
    vec2 uvStart;
    vec2 uvSize;
};

layout(binding = 0) uniform sampler2D font_map;

void main(){

    vec2 UV = _UV * uvSize + uvStart;

    vec4 color = texture(font_map, UV).r * vec4(text_color, 1.0);

    FragColor = color;

}