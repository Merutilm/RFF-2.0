#version 450
#define NONE 0
#define NORMAL 1
#define REVERSED 2


uniform ivec2 resolution;
uniform double maxIteration;

uniform sampler2D normalAndZoomed;
uniform float defaultZoomIncrement;
uniform float currentFrame;

uniform sampler2D palette;
uniform int paletteWidth;
uniform int paletteHeight;
uniform int paletteLength;
uniform double paletteOffset;
uniform float paletteInterval;

uniform int smoothing;

out vec4 color;

double getIterationNormal(ivec2 coord){
    vec4 iteration = texelFetch(normalAndZoomed, ivec2(coord), 0);
    uvec2 ith = floatBitsToUint(iteration.yx);
    double result = packDouble2x32(ith);
    if (result >= maxIteration){
        return maxIteration * 2;// smooth plane
    }
    return result;
}

double getIterationZoomed(ivec2 coord){
    vec4 iteration = texelFetch(normalAndZoomed, ivec2(coord), 0);
    uvec2 ith = floatBitsToUint(iteration.wz);
    double result = packDouble2x32(ith);
    if (result >= maxIteration){
        return maxIteration * 2;// smooth plane
    }
    return result;
}

vec4 getColor(double iteration){

    if (iteration == 0 || iteration >= maxIteration){
        return vec4(0, 0, 0, 1);
    }
    switch (smoothing){
        case NONE :
        iteration = iteration - mod(iteration, 1);
        break;
        case NORMAL :
        break;
        case REVERSED :
        iteration = iteration + 1 - 2 * mod(iteration, 1);
        break;
    }


    float hSquare = paletteHeight - 1;
    float hRemainder = (paletteLength - paletteWidth * hSquare) / paletteWidth;
    float hLength = hSquare + hRemainder;

    double offset = mod(iteration / paletteInterval + paletteOffset, 1) * hLength;

    float ox = float(mod(offset, 1));
    float oy = float((floor(offset) + 0.5) / paletteHeight);

    return texture(palette, vec2(ox ,oy));
}

void main(){

    vec2 coord = gl_FragCoord.xy;
    vec2 center = vec2(resolution) / 2.0;
    coord -= center;
    float r = int(max(0, currentFrame)) - currentFrame;

    float nsr = pow(defaultZoomIncrement, r + 1);// r = 0 ~ 1
    float zsr = pow(defaultZoomIncrement, r);// r = -1 ~ 0

    vec2 ntx = coord / nsr + center;
    vec2 ztx = coord / zsr + center;

    vec2 nrt = mod(ntx, 1);
    vec2 zrt = mod(ztx, 1);

    double nMain = getIterationNormal(ivec2(ntx));
    double zMain = getIterationZoomed(ivec2(ztx));

    vec4 nc1 = getColor(nMain);
    vec4 nc2 = getColor(getIterationNormal(ivec2(ntx) + ivec2(1, 0)));
    vec4 nc3 = getColor(getIterationNormal(ivec2(ntx) + ivec2(0, 1)));
    vec4 nc4 = getColor(getIterationNormal(ivec2(ntx) + ivec2(1, 1)));

    vec4 nc5 = nc1 - (nc1 - nc2) * nrt.x;
    vec4 nc6 = nc3 - (nc3 - nc4) * nrt.x;
    vec4 nc7 = nc5 - (nc5 - nc6) * nrt.y;

    if (ztx.x >= resolution.x - 1 || ztx.y >= resolution.y - 1 || ztx.x < 0 || ztx.y < 0 || zMain == 0){
        color = nc7;
    } else {
        vec4 zc1 = getColor(zMain);
        vec4 zc2 = getColor(getIterationZoomed(ivec2(ztx) + ivec2(1, 0)));
        vec4 zc3 = getColor(getIterationZoomed(ivec2(ztx) + ivec2(0, 1)));
        vec4 zc4 = getColor(getIterationZoomed(ivec2(ztx) + ivec2(1, 1)));

        vec4 zc5 = zc1 - (zc1 - zc2) * zrt.x;
        vec4 zc6 = zc3 - (zc3 - zc4) * zrt.x;
        vec4 zc7 = zc5 - (zc5 - zc6) * zrt.y;

        color = nc7 * (-r) + zc7 * (r + 1);
    }
    
}
