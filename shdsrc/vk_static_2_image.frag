#version 450

layout (set = 0, binding = 0) uniform sampler2D normal;
layout (set = 0, binding = 1) uniform sampler2D zoomed;

layout (set = 1, binding = 0) uniform VideoUBO {
    float default_zoom_increment;
    float current_frame;
} video_attr;


layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main(){
    vec2 resolution = vec2(textureSize(normal, 0));
    vec2 coord = gl_FragCoord.xy / resolution;

    float r = int(max(0, video_attr.current_frame)) - video_attr.current_frame;

    float nsr = pow(video_attr.default_zoom_increment, r + 1);// r = 0 ~ 1
    float zsr = pow(video_attr.default_zoom_increment, r);// r = -1 ~ 0


    int off = 3;
    vec2 ntx = (coord - 0.5) / nsr + 0.5;
    vec2 ztx = (coord - 0.5) / zsr + 0.5;
    vec2 px = off / resolution;

    if (ztx.x >= 1 - px.x || ztx.y >= 1 - px.y || ztx.x <= px.x || ztx.y <= px.y || video_attr.current_frame < 1){
        color = texture(normal, ntx).bgra;
    } else {
        color = texture(normal, ntx).bgra * (-r) + texture(zoomed, ztx).bgra * (r + 1);
    }

}