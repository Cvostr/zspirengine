#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UVCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D tDiffuse;

void main() {
	vec3 Diffuse = texture(tDiffuse, UVCoord).rgb;

	outColor = vec4(Diffuse, 1);
}