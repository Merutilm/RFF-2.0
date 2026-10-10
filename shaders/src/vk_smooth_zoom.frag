#version 450
#include <common.glsl>

// define descriptors
#define DESC_SMOOTH_ZOOM 1
#define DESC_ITERATION 2

// include descriptors
#include <desc_smooth_zoom.glsl>
#include <desc_iteration.glsl>

// include utilities
#include <utils_iteration.glsl>


layout(set = 0, binding = 0) uniform sampler2D canvas;

layout (set = 3, binding = 1) writeonly buffer SnapshotSSBO {
    double iterations[];
} snapshot_settings;





layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 color;

void main() {

    ivec2 curr_iter_coord = ivec2(gl_FragCoord.xy);
    vec2 coord = gl_FragCoord.xy / iteration_info_settings.extent;

    coord = (coord - 0.5f) / pow(10, float(smooth_zoom_settings.log_zoom_delta)) - smooth_zoom_settings.pos_delta + 0.5f;
    color = texture(canvas, coord);

    ivec2 iter_coord = ivec2(coord * iteration_info_settings.extent);

    if(iter_coord.x < 0 || iter_coord.y < 0 || iter_coord.x >= iteration_info_settings.extent.x || iter_coord.y >= iteration_info_settings.extent.y) {
        snapshot_settings.iterations[get_iteration_index(curr_iter_coord)] = 0;
        return;
    }
    snapshot_settings.iterations[get_iteration_index(curr_iter_coord)] = get_iteration(iter_coord);
}