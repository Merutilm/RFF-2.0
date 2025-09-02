#version 450

layout (set = 0, binding = 0) uniform sampler2D canvas;

layout (set = 1, binding = 0) uniform ResolutionUBO{
    uvec2 swapchain_extent;
    float clarity_multiplier;
} resolution_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {

    color = texture(canvas, vec2(gl_FragCoord.xy / resolution_attr.swapchain_extent));
}
