#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 pos;

void main() {
	vec4 color = {1.0, 0.0, 0.0, 1.0};
    outColor = color;
}