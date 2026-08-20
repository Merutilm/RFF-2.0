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
#include <utils_palette.glsl>


layout (set = 3, binding = 0) uniform CameraUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} camera_settings;

layout(set = 4, binding = 0) uniform Fractal3DUBO{
    float base_iteration;
    float depth_divisor;
    float rotation_rad;
} fractal_3d_settings;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexcoord;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexcoord;


void main() {
    uint w = iteration_info_settings.extent.x;
    uint h = iteration_info_settings.extent.y;
    uint x = gl_VertexIndex % w;
    uint y = gl_VertexIndex / w;
    double iteration = iteration_settings.iterations[gl_VertexIndex];

    float nx = (float(x) / w  * 2 - 1);
    float ny = 1 - float(y) / h * 2;
    float nz = 1 - float(iteration - fractal_3d_settings.base_iteration) / fractal_3d_settings.depth_divisor;

    float aspect = float(w) / h;

    float rx = nx * cos(fractal_3d_settings.rotation_rad) + ny * sin(fractal_3d_settings.rotation_rad) / aspect;
    float ry = ny * cos(fractal_3d_settings.rotation_rad) - nx * sin(fractal_3d_settings.rotation_rad) * aspect;

    vec4 world_position = vec4(rx, ry, nz, 1.0);
    gl_Position = camera_settings.proj * camera_settings.view * camera_settings.model * world_position;

    fragColor = palette_get_color(iteration).rgb;
    fragTexcoord = inTexcoord;
}