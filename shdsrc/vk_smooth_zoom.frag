#version 450


layout(set = 0, binding = 0) uniform sampler2D canvas;

layout(set = 1, binding = 0) uniform SmoothZoomUBO{
    vec2 pos_delta;
    float log_zoom_delta;
} smooth_zoom_settings;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 color;

void main() {
    
}