#version 450
#include <common.glsl>

// define descriptors
#define DESC_SMOOTH_ZOOM 1

// include descriptors
#include <desc_smooth_zoom.glsl>

// include utilities


layout(set = 0, binding = 0) uniform sampler2D canvas;

layout (set = 0, binding = 1) uniform ResampleUBO{
    uvec2 extent;
} resample_settings;




layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 color;

void main() {

    vec2 coord = gl_FragCoord.xy / resample_settings.extent;
    coord = (coord - 0.5f) / pow(10, smooth_zoom_settings.log_zoom_delta) - smooth_zoom_settings.pos_delta + 0.5f;
    color = texture(canvas, coord);
}