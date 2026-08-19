#version 450
#include <common.glsl>

// define descriptors

// include descriptors

// include utilities

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {
    color = vec4(fragColor, 1);
}