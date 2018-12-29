#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat3 model;
    mat3 view;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
	vec3 pos = ubo.view * ubo.model * vec3(inPosition, 1);
    gl_Position = vec4 (inPosition.xy, 0, 1);
    fragColor = inColor;
	//fragColor.r = ubo.model[2][1];
	//fragColor.g = ubo.model[2][0];
	//fragColor.b = ubo.model[1][0];
	
	fragColor.r = ubo.model[0][0];
	fragColor.g = ubo.model[1][1];
	fragColor.b = ubo.model[2][2];
}