#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 1
#define DESC_STRIPE 2
#define DESC_TIME 3

// include descriptors
#include <desc_iteration.glsl>
#include <desc_stripe.glsl>
#include <desc_time.glsl>

// include utilities
#include <utils_iteration.glsl>
#include <utils_stripe.glsl>

layout (set = 0, binding = 0) uniform sampler2D canvas;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;


void main() {

    ivec2 iter_coord = ivec2(gl_FragCoord.xy);
    double iteration = get_iteration(iter_coord);

    if (stripe_settings.type == ST_NONE || iteration == 0) {
        color = texelFetch(canvas, ivec2(gl_FragCoord.xy), 0);
        return;
    }

    float multiplier = stripe_get_multiplier(iteration);

    color = vec4(texelFetch(canvas, ivec2(gl_FragCoord.xy), 0).rgb * multiplier, 1);
}
