#version 450

layout (set = 0, binding = 0) uniform sampler2D canvas;
layout (set = 0, binding = 1) uniform SampleUBO {
    uvec2 extent;
} sample_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {
    color = texture(canvas, vec2(gl_FragCoord.xy / sample_attr.extent));
}
