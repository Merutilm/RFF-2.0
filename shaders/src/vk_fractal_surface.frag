#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 1
#define DESC_FRACTAL_SURFACE 2
#define DESC_TIME 3
// include descriptors
#include <desc_iteration.glsl>
#include <desc_fractal_surface.glsl>
#include <desc_time.glsl>

// include utilities
#include <utils_iteration.glsl>
#include <utils_fractal_surface.glsl>

layout (set = 0, binding = 0) uniform sampler2D canvas;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;


void main() {

    ivec2 iter_coord = ivec2(gl_FragCoord.xy);
    color = vec4(surface_get_shade(canvas, iter_coord), 1);
}
