#version 450
#extension GL_ARB_gpu_shader_int64 : enable


uniform ivec2 resolution;
uniform double maxIteration;

uniform sampler2D normalAndZoomed;
uniform float defaultZoomIncrement;
uniform float currentFrame;

out vec4 color;



vec2 toFloatIteration(double iteration){
    uvec2 unpack = unpackDouble2x32(iteration);
    return uintBitsToFloat(unpack.yx);
}

double getIterationNormal(ivec2 coord){
    vec4 iteration = texelFetch(normalAndZoomed, ivec2(coord), 0);
    uvec2 ith = floatBitsToUint(iteration.yx);
    double result = packDouble2x32(ith);
    if(result >= maxIteration){
        return maxIteration * 2; // smooth mandelbrot plane
    }
    return result;
}

double getIterationZoomed(ivec2 coord){
    vec4 iteration = texelFetch(normalAndZoomed, ivec2(coord), 0);
    uvec2 ith = floatBitsToUint(iteration.wz);
    double result = packDouble2x32(ith);
    if(result >= maxIteration){
        return maxIteration * 2; // smooth mandelbrot plane
    }
    return result;
}

void main(){

    vec2 coord = gl_FragCoord.xy;
    vec2 center = vec2(resolution) / 2.0;
    coord -= center;
    float r = int(max(0, currentFrame)) - currentFrame;

    float nsr = pow(defaultZoomIncrement, r + 1); // r = 0 ~ 1
    float zsr = pow(defaultZoomIncrement, r); // r = -1 ~ 0

    vec2 ntx = coord / nsr + center;
    vec2 ztx = coord / zsr + center;

    vec2 nrt = mod(ntx, 1);
    vec2 zrt = mod(ztx, 1);

    double nMain = getIterationNormal(ivec2(ntx));
    double zMain = getIterationZoomed(ivec2(ztx));

    if(ztx.x >= resolution.x - 1 || ztx.y >= resolution.y - 1 || ztx.x < 0 || ztx.y < 0 || zMain == 0){
        double i2 = getIterationNormal(ivec2(ntx) + ivec2(1, 0));
        double i3 = getIterationNormal(ivec2(ntx) + ivec2(0, 1));
        double i4 = getIterationNormal(ivec2(ntx) + ivec2(1, 1));

        double i5 = nMain - (nMain - i2) * nrt.x;
        double i6 = i3 - (i3 - i4) * nrt.x;
        double i7 = i5 - (i5 - i6) * nrt.y;
        vec2 it = toFloatIteration(i7);

        color = vec4(it, 0, 0);
    }else{
        double i2 = getIterationZoomed(ivec2(ztx) + ivec2(1, 0));
        double i3 = getIterationZoomed(ivec2(ztx) + ivec2(0, 1));
        double i4 = getIterationZoomed(ivec2(ztx) + ivec2(1, 1));

        double i5 = zMain - (zMain - i2) * zrt.x;
        double i6 = i3 - (i3 - i4) * zrt.x;
        double i7 = i5 - (i5 - i6) * zrt.y;
        vec2 it = toFloatIteration(i7);

        color = vec4(it, 0, 0);
    }

}