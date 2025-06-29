#version 450
#extension GL_ARB_gpu_shader_int64 : enable


uniform ivec2 resolution;

uniform sampler2D normal;
uniform sampler2D zoomed;
uniform float defaultZoomIncrement;
uniform float currentFrame;

out vec4 color;

void main(){

    vec2 coord = gl_FragCoord.xy / vec2(resolution);

    float r = int(max(0, currentFrame)) - currentFrame;

    float nsr = pow(defaultZoomIncrement, r + 1);// r = 0 ~ 1
    float zsr = pow(defaultZoomIncrement, r);// r = -1 ~ 0


    int off = 3;
    vec2 ntx = (coord - 0.5) / nsr + 0.5;
    vec2 ztx = (coord - 0.5) / zsr + 0.5;
    vec2 px = off / vec2(resolution);

    if (ztx.x >= 1 - px.x || ztx.y >= 1 - px.y || ztx.x <= px.x || ztx.y <= px.y || currentFrame < 1){
        color = texture(normal, ntx);
    } else {
        color = texture(normal, ntx) * (-r) + texture(zoomed, ztx) * (r + 1);
    }

}