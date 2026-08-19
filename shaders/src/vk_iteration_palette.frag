#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 0
#define DESC_PALETTE 1
#define DESC_TIME 2

// include descriptors
#include <desc_iteration.glsl>
#include <desc_palette.glsl>
#include <desc_time.glsl>

// include utilities
#include <utils_iteration.glsl>
#include <utils_palette.glsl>


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {

    ivec2 iter_coord = ivec2(gl_FragCoord.xy);

    float x = iter_coord.x;
    float y = iter_coord.y;

    double iteration = get_iteration(iter_coord);

    if (iteration == 0) {
        color = vec4(0, 0, 0, 1);
        return;
    }

    color = palette_get_color(iteration);
}