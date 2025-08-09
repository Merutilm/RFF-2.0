#version 450
#define NONE 0
#define NORMAL 1
#define REVERSED 2

layout(set = 0, binding = 0) uniform sampler2D iterations;
layout(set = 0, binding = 1) uniform IterUBO{
    double max_value;
} iteration_attr;

layout(set = 1, binding = 0) uniform sampler2D palette;
layout(set = 1, binding = 1) uniform PaletteUBO {
    int width;
    int height;
    int size;
    float interval;
    double offset;
    int smoothing;
} palette_attr;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 color;


double getIteration0(ivec2 iterCoord){
    vec4 iter_raw = texelFetch(iterations, iterCoord, 0);
    uvec2 iter_uintbits = floatBitsToUint(iter_raw.yx);
    double result = packDouble2x32(iter_uintbits);
    if (result >= iteration_attr.max_value){
        return iteration_attr.max_value * 2;// smooth mandelbrot plane
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

    if (i5 == 0){
        i5 = i6;
    }
    if (i6 == 0){
        i6 = i5;
    }

    return i5 - (i5 - i6) * dec.y;
}

vec4 getColor(double iteration){

    if (iteration == 0 || iteration >= iteration_attr.max_value){
        return vec4(0, 0, 0, 1);
    }
    switch (palette_attr.smoothing){
        case NONE :
        iteration = iteration - mod(iteration, 1);
        break;
        case NORMAL :
        break;
        case REVERSED :
        iteration = iteration + 1 - 2 * mod(iteration, 1);
        break;
    }


    float hSquare = palette_attr.height - 1;
    float hRemainder = (palette_attr.size - palette_attr.width * hSquare) / palette_attr.width;
    float hLength = hSquare + hRemainder;

    double offset = mod(iteration / palette_attr.interval + palette_attr.offset, 1) * hLength;

    float ox = float(mod(offset, 1));
    float oy = float((floor(offset) + 0.5) / palette_attr.height);

    return texture(palette, vec2(ox, oy));
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