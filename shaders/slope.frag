#version 450
#define PI 3.141592653589793238

uniform sampler2D inputTex;
uniform sampler2D iterations;

uniform float clarityMultiplier;
uniform float depth;
uniform float reflectionRatio;
uniform float opacity;
uniform float zenith;
uniform float azimuth;

in vec4 fColor;
out vec4 color;

double getIteration0(ivec2 iterCoord){
    vec4 iteration = texelFetch(iterations, iterCoord, 0);
    uvec2 it = floatBitsToUint(iteration.yx);
    return packDouble2x32(it);
}

double getIteration(vec2 coord){
    vec2 dec = mod(coord, 1);

    double i1 = getIteration0(ivec2(coord));
    double i2 = getIteration0(ivec2(coord) + ivec2(1, 0));
    double i3 = getIteration0(ivec2(coord) + ivec2(0, 1));
    double i4 = getIteration0(ivec2(coord) + ivec2(1, 1));


    double i5 = i1 - (i1 - i2) * dec.x;
    double i6 = i3 - (i3 - i4) * dec.x;

    if(i5 == 0){
        i5 = i6;
    }
    if(i6 == 0){
        i6 = i5;
    }


    return i5 - (i5 - i6) * dec.y;
}


void main() {

    vec2 coord = gl_FragCoord.xy;

    if(reflectionRatio >= 1 || depth == 0){
        color = texelFetch(inputTex, ivec2(coord), 0);
        return;
    }


    float aRad = radians(azimuth);
    float zRad = radians(zenith);

    double ld = getIteration(coord + vec2(-1, -1));
    double d = getIteration(coord + vec2(0, -1));
    double rd = getIteration(coord + vec2(1, -1));
    double l = getIteration(coord + vec2(-1, 0));
    double r = getIteration(coord + vec2(1, 0));
    double lu = getIteration(coord + vec2(-1, 1));
    double u = getIteration(coord + vec2(0, 1));
    double ru = getIteration(coord + vec2(1, 1));

    float dzDx = float((rd + 2 * r + ru) - (ld + 2 * l + lu)) * depth * clarityMultiplier;
    float dzDy = float((lu + 2 * u + ru) - (ld + 2 * d + rd)) * depth * clarityMultiplier;
    float slope = atan(radians(length(vec2(dzDx, dzDy))), 1);
    float aspect = atan(dzDy, -dzDx);
    float shade = max(reflectionRatio, cos(zRad) * cos(slope) + sin(zRad) * sin(slope) * cos(aRad + aspect));
    float fShade = 1 - opacity * (1 - shade);


    color = vec4(texelFetch(inputTex, ivec2(coord), 0).rgb * fShade, 1);
}
