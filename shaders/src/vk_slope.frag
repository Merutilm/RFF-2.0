#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 1
#define DESC_SLOPE 2

// include descriptors
#include <desc_iteration.glsl>
#include <desc_slope.glsl>

// include utilities
#include <utils_iteration.glsl>
#include <utils_slope.glsl>

layout (set = 0, binding = 0) uniform sampler2D canvas;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {

    ivec2 iter_coord = ivec2(gl_FragCoord.xy);
    float shade = slope_get_shade(iter_coord);


    color = vec4(texelFetch(canvas, ivec2(iter_coord), 0).rgb * shade, 1);
}
