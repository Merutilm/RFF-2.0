#version 450

layout (set = 0, binding = 0) uniform sampler2D canvas;
layout (set = 0, binding = 1) uniform sampler2D blurred;
layout (set = 1, binding = 0) uniform BloomUBO {
    float threshold;
    float radius;
    float softness;
    float intensity;
} bloom_attr;

layout (set = 2, binding = 0) uniform ResolutionUBO {
    uvec2 swapchain_extent;
    float clarity_multiplier;
} resolution_attr;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;


void main() {

    vec2 coord = gl_FragCoord.xy / resolution_attr.swapchain_extent / resolution_attr.clarity_multiplier;

    float x = coord.x;
    float y = coord.y;

    if (x < 0 || y < 0){
        discard;
    }

    if (x >= 1 || y >= 1){
        discard;
    }

    vec4 c = texture(canvas, coord);
    vec3 blur = texture(blurred, coord).rgb;
    vec3 add = blur - (blur - c.rgb) * bloom_attr.softness;
    color = c + vec4(add * bloom_attr.intensity, 1);

}
