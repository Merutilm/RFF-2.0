#version 450

layout (set = 0, binding = 0) uniform sampler2D canvas;
layout (set = 0, binding = 1) uniform ResampleUBO{
    uvec2 extent;
} resample_attr;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {

    vec2 coord = vec2(gl_FragCoord.xy / resample_attr.extent);
    color = texture(canvas, coord);
}
