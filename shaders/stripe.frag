#version 450

#define DOUBLE_PI 6.2831853071795864
#define NONE 0
#define SINGLE_DIRECTION 1
#define SMOOTH 2
#define SMOOTH_SQUARED 3

uniform sampler2D inputTex;
uniform sampler2D iterations;

uniform int type;
uniform float firstInterval;
uniform float secondInterval;
uniform float opacity;
uniform float offset;

in vec4 fColor;
out vec4 color;


double getIteration0(ivec2 iterCoord) {
    vec4 iteration = texelFetch(iterations, iterCoord, 0);
    uvec2 it = floatBitsToUint(iteration.yx);
    return packDouble2x32(it);
}

double getIteration(vec2 coord) {

    vec2 dec = mod(coord, 1);
    double i1 = getIteration0(ivec2(coord));
    double i2 = getIteration0(ivec2(coord) + ivec2(1, 0));
    double i3 = getIteration0(ivec2(coord) + ivec2(0, 1));
    double i4 = getIteration0(ivec2(coord) + ivec2(1, 1));

    double i5 = i1 - (i1 - i2) * dec.x;
    double i6 = i3 - (i3 - i4) * dec.x;

    if (i5 == 0) {
        i5 = i6;
    }
    if (i6 == 0) {
        i6 = i5;
    }

    return i5 - (i5 - i6) * dec.y;
}


void main() {
    vec2 coord = gl_FragCoord.xy;

    double iteration = getIteration(coord);
    color = texelFetch(inputTex, ivec2(coord), 0);

    if (type == NONE || iteration == 0) {
        return;
    }
    double iterCurr = iteration - offset;
    float black;
    float rat1 = float(mod(iterCurr, firstInterval)) / firstInterval;
    float rat2 = float(mod(iterCurr, secondInterval)) / secondInterval;

    switch (type) {
        case SINGLE_DIRECTION: {
                                   black = rat1 * rat2;
                                   break;
                               }
        case SMOOTH: {
                                   black = pow((sin(rat1 * DOUBLE_PI) + 1) * (sin(rat2 * DOUBLE_PI) + 1) / 4, 2);
                                   break;
                               }
        case SMOOTH_SQUARED: {
                                   black = pow((sin(rat1 * DOUBLE_PI) + 1) * (sin(rat2 * DOUBLE_PI) + 1) / 4, 4);
                                   break;
                               }
    }


    color = vec4((color * (1 - black * opacity)).rgb, 1);
}
