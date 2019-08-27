#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 pos;

void main() {
	vec3 outlineColor = {0.5, 0.5, 0.5};
	vec3 color = {0.75, 0.75, 0.75};
	float outlineSize = 0.99f;
	float inside = float(pos.x > -outlineSize && pos.x < outlineSize && pos.y > -outlineSize && pos.y < outlineSize);
    outColor = vec4(color * inside + outlineColor * (1 - inside), 1.0);
}