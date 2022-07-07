#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vNormal;

uniform mat4 world, view, projection;

out vec4 worldPixel;

void main() {
	worldPixel = world * vec4(vPos, 1.0f);
	gl_Position = projection * view * world * vec4(vPos, 1.0f);
}