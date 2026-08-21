#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 0
#define DESC_PALETTE 1
#define DESC_TIME 2
#define DESC_SLOPE 6

// include descriptors
#include <desc_iteration.glsl>
#include <desc_palette.glsl>
#include <desc_time.glsl>
#include <desc_slope.glsl>

// include utilities
#include <utils_iteration.glsl>
#include <utils_palette.glsl>


layout (set = 3, binding = 0) uniform CameraUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} camera_settings;

layout (set = 4, binding = 0) uniform Fractal3DUBO {
    float base_iteration;
    float depth_divisor;
    float rotation_rad;
} fractal_3d_settings;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexcoord;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexcoord;
layout (location = 2) out vec3 fragNormal;
layout (location = 3) out float fragIterationHi;
layout (location = 4) out float fragIterationLo;


void main() {
    uint w = iteration_info_settings.extent.x;
    uint h = iteration_info_settings.extent.y;
    uint x = gl_VertexIndex % w;
    uint y = gl_VertexIndex / w;

    ivec2 iter_coord = ivec2(x, h - y - 1);

    double iteration = iteration_settings.iterations[gl_VertexIndex];

    float nx = float(x) / w * 2 - 1;
    float ny = float(y) / h * 2 - 1;
    float nz = 1 - float(iteration - fractal_3d_settings.base_iteration) / fractal_3d_settings.depth_divisor;

    nx *= float(w) / h;

    uint leftIndex = (x > 0) ? gl_VertexIndex - 1 : gl_VertexIndex;
    uint rightIndex = (x + 1 < w) ? gl_VertexIndex + 1 : gl_VertexIndex;
    uint topIndex = (y > 0) ? gl_VertexIndex - w : gl_VertexIndex;
    uint bottomIndex = (y + 1 < h) ? gl_VertexIndex + w : gl_VertexIndex;

    double ld = get_iteration(iter_coord, ivec2(-1, -1));
    double d = get_iteration(iter_coord, ivec2(0, -1));
    double rd = get_iteration(iter_coord, ivec2(1, -1));
    double l = get_iteration(iter_coord, ivec2(-1, 0));
    double r = get_iteration(iter_coord, ivec2(1, 0));
    double lu = get_iteration(iter_coord, ivec2(-1, 1));
    double u = get_iteration(iter_coord, ivec2(0, 1));
    double ru = get_iteration(iter_coord, ivec2(1, 1));

    vec4 world_position = vec4(nx, ny, nz, 1.0);
    float multiplier = float(w) / 1280;

    gl_Position = camera_settings.proj * camera_settings.view * camera_settings.model * world_position;

    float depth = -slope_settings.depth;
    double left = ld + 2 * l + lu;
    double right = rd + 2 * r + ru;
    double top = lu + 2 * u + ru;
    double bottom = ld + 2 * d + rd;

    fragIterationHi = float(iteration);
    fragIterationLo = float(iteration - double(fragIterationHi));

    fragNormal = normalize(vec3((left - right) * depth * multiplier, (bottom - top) * depth * multiplier, 1.0));
    fragColor = palette_get_color(iteration).rgb;
    fragTexcoord = inTexcoord;
}