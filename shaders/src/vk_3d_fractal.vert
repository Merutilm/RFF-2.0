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

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexcoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexcoord;


void main(){
    float x = float(gl_VertexIndex % iteration_info_settings.extent.x);
    float y = float(gl_VertexIndex / iteration_info_settings.extent.x);
    float z = float(iteration_settings.iterations[gl_VertexIndex]);

    gl_Position = camera_settings.proj * camera_settings.view * camera_settings.model * vec4(x, y, z, 1.0);
    fragColor = palette_get_color(iteration_settings.iterations[gl_VertexIndex]).rgb;
    fragTexcoord = inTexcoord;
}