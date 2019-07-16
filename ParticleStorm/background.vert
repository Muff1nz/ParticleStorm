#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 pos;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 0.0, 1.0);
	pos = inPosition;
    fragColor = inColor;
}