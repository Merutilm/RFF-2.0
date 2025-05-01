#version 450
#extension GL_ARB_gpu_shader_int64 : enable
#define NONE 0
#define NORMAL 1
#define REVERSED 2



uniform sampler2D iterations;
uniform double maxIteration;
uniform float resolutionMultiplier;

uniform sampler2D palette;
uniform int paletteWidth;
uniform int paletteHeight;
uniform int paletteLength;
uniform float paletteOffset;
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
    vec2 iterCoord = coord * resolutionMultiplier;
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
        iteration = int64_t(iteration);
        break;
        case NORMAL :
        break;
        case REVERSED :
        iteration = 2 * int64_t(iteration) + 1 - iteration;
        break;
    }
   
    
    float hSquare = paletteHeight - 1;
    float hRemainder = (paletteLength - paletteWidth * hSquare) / paletteWidth;
    float hLength = hSquare + hRemainder;

    float offset = mod(float(iteration / paletteInterval + paletteOffset), 1) * hLength;
    
    float ox = mod(offset, 1);
    float oy = (floor(offset) + 0.5) / paletteHeight;
    
    return texture(palette, vec2(ox ,oy));
}

void main(){

    vec2 coord = gl_FragCoord.xy;

    float x = coord.x;
    float y = coord.y;

    double iteration = getIteration(coord);

    if (iteration == 0){

        double it2 = 0;
        while (coord.y > 0){
            coord.y -= 1.0;
            it2 = getIteration(coord);
            if (it2 > 0){
                break;
            }
        }
        color = getColor(it2);
        return;
    }

    color = getColor(iteration);
}