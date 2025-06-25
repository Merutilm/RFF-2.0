#version 450
#define NONE 0
#define NORMAL 1
#define REVERSED 2

uniform sampler2D iterations;
uniform double maxIteration;

uniform sampler2D palette;
uniform int paletteWidth;
uniform int paletteHeight;
uniform int paletteLength;
uniform double paletteOffset;
uniform float paletteInterval;

uniform int smoothing;

out vec4 color;


double getIteration0(ivec2 iterCoord){
    vec4 iteration = texelFetch(iterations, iterCoord, 0);
    uvec2 it = floatBitsToUint(iteration.yx);
    double result = packDouble2x32(it);
    if(result >= maxIteration){
        return maxIteration * 2; // smooth mandelbrot plane
    }
    return result;
}

double getIteration(vec2 coord){
    vec2 iterCoord = coord;
    vec2 dec = mod(iterCoord, 1);

    double i1 = getIteration0(ivec2(iterCoord));
    double i2 = getIteration0(ivec2(iterCoord) + ivec2(1, 0));
    double i3 = getIteration0(ivec2(iterCoord) + ivec2(0, 1));
    double i4 = getIteration0(ivec2(iterCoord) + ivec2(1, 1));

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

    float x = coord.x;
    float y = coord.y;

    double iteration = getIteration(coord);

    if (iteration == 0){
        discard;
    }

    color = getColor(iteration);
}