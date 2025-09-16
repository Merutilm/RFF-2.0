#version 450

layout(set = 0, binding = 0) uniform sampler2D canvas;
layout(set = 1, binding = 0) uniform LinearInterpolationUBO{
    bool use;
} linear_interpolation_attr;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 color;

void main() {

    ivec2 coord = ivec2(gl_FragCoord.xy);

    if (!linear_interpolation_attr.use) {
        color = texelFetch(canvas, coord, 0);
        return;
    }
    vec4 c1 = texelFetch(canvas, coord + ivec2(1, 0), 0);
    vec4 c2 = texelFetch(canvas, coord + ivec2(0, 1), 0);
    vec4 c3 = texelFetch(canvas, coord + ivec2(-1, 0), 0);
    vec4 c4 = texelFetch(canvas, coord + ivec2(0, -1), 0);
    vec4 c5 = texelFetch(canvas, coord + ivec2(1, 1), 0);
    vec4 c6 = texelFetch(canvas, coord + ivec2(1, -1), 0);
    vec4 c7 = texelFetch(canvas, coord + ivec2(-1, 1), 0);
    vec4 c8 = texelFetch(canvas, coord + ivec2(-1, -1), 0);
    vec4 center = texelFetch(canvas, coord, 0) * 9;
    vec4 near = (c1 + c2 + c3 + c4) * 3;
    vec4 diagonal = (c5 + c6 + c7 + c8);

    color = (center + near + diagonal) / 25;

}
