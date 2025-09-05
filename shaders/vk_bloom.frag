#version 450

layout (set = 0, binding = 0) uniform sampler2D bloom_canvas;
layout (set = 0, binding = 1) uniform sampler2D bloom_blurred;
layout (set = 1, binding = 0) uniform BloomUBO {
    float threshold;
    float radius;
    float softness;
    float intensity;
} bloom_attr;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;


void main() {

    vec2 coord = gl_FragCoord.xy / textureSize(bloom_canvas, 0);

    float x = coord.x;
    float y = coord.y;

    if (x < 0 || y < 0){
        discard;
    }

    if (x >= 1 || y >= 1){
        discard;
    }

    color = texture(bloom_canvas, coord);
    vec3 blur = texture(bloom_blurred, coord).rgb;
    vec3 add = blur - (blur - color.rgb) * bloom_attr.softness;
    color = color + vec4(add * bloom_attr.intensity, 1);

}
