#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput canvas;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {

    vec3 c = subpassLoad(canvas).rgb;
    color = vec4(c, 1);
}
