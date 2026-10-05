#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 0
#define DESC_TIME 2
#define DESC_STRIPE 5
#define DESC_FRACTAL_SURFACE 6

// include descriptors
#include <desc_iteration.glsl>
#include <desc_stripe.glsl>
#include <desc_fractal_surface.glsl>

// include utilities
#include <utils_iteration.glsl>
#include <utils_stripe.glsl>

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;
layout (location = 2) in vec3 fragNormal;
layout (location = 3) in float fragIterationHi;
layout (location = 4) in float fragIterationLo;

layout (location = 0) out vec4 color;

void main() {

    float azimuth_rad = radians(surface_settings.light_azimuth);
    float zenith_rad = radians(surface_settings.light_zenith);
    vec3 light_direction = vec3(cos(azimuth_rad) * cos(zenith_rad), sin(azimuth_rad) * cos(zenith_rad), cos(zenith_rad));
    vec3 n = normalize(fragNormal);
    vec3 l = normalize(light_direction);
    float diffuse = max(dot(n, l), surface_settings.shadow_brightness);
    float opacity = surface_settings.shadow_opacity;

    double iteration = double(fragIterationHi) + double(fragIterationLo);

    float stripe_multiplier = stripe_get_multiplier(iteration);

    color = vec4(fragColor * ((1 - opacity) + opacity * diffuse) * stripe_multiplier, 1);
}