#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 pos;

void main() {
	vec4 outlineColor = {0.5, 0.5, 0.5, 1};
	float outlineSize = 0.25f;

	float len = length(pos);
	float inCore = float(len <= (1 - outlineSize));
	float inOutline = float(len > (1 - outlineSize) && len <= 1);
    outColor = vec4(fragColor, 1.0) * inCore + outlineColor * inOutline;
}