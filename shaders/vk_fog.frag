#version 450

layout (set = 0, binding = 0) uniform sampler2D fog_canvas;
layout (set = 0, binding = 1) uniform sampler2D fog_blurred;

layout (set = 1, binding = 0) uniform FogUBO {
    float radius;
    float opacity;
} fog_attr;

layout (set = 2, binding = 0) uniform ResolutionUBO {
    uvec2 swapchain_extent;
    float clarity_multiplier;
} resolution_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

float grayScale(vec3 c) {
    return c.r * 0.3 + c.g * 0.59 + c.b * 0.11;
}

void main() {

    vec2 coord = gl_FragCoord.xy / resolution_attr.swapchain_extent / resolution_attr.clarity_multiplier;

    float x = coord.x;
    float y = coord.y;

    if (x < 0 || y < 0) {
        discard;
    }
    if (x >= 1 || y >= 1) {
        discard;
    }
    vec3 blurredColor = texture(fog_blurred, coord).rgb;
    color = texture(fog_canvas, coord);

    vec3 cf = color.rgb - (color.rgb - blurredColor) * fog_attr.opacity;

    float cg = grayScale(color.rgb);
    float cfg = grayScale(cf);

    if (cg < cfg) {
        color = vec4(cf, 1);
    }

}
