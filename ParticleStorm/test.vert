#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in mat4 mvp;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = mvp * vec4(inPosition, 0.0, 1.0);
	gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
	fragColor.r = mvp[0][0];
	fragColor.g = mvp[1][1];
	fragColor.b = mvp[2][2];
}