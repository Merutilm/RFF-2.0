#version 450

layout(push_constant) uniform Push{
    vec2 resolution;
}push;


layout(set = 0, binding = 0) uniform Time{
    float time;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D sampled;
layout(input_attachment_index = 0, set = 1, binding = 1) uniform subpassInput img;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;
layout (location = 0) out vec4 color;


void main(){
    vec3 mul = vec3(sin(gl_FragCoord.x / push.resolution.x * 2 + ubo.time) / 2 + 0.5, sin(gl_FragCoord.y / push.resolution.y * 2 + ubo.time * 1.198) / 2 + 0.5, sin(gl_FragCoord.y / push.resolution.y * 2 + ubo.time * 1.321) / 2 + 0.5);
    color = vec4(texture(sampled, gl_FragCoord.xy/push.resolution).rgb * mul, 1);
}