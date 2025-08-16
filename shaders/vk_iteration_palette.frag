#version 450
#define NONE 0
#define NORMAL 1
#define REVERSED 2

layout (set = 0, binding = 0) buffer IterSSBO {
    uvec2 extent;
    double max_value;
    double iterations[];
} iteration_attr;

layout (set = 1, binding = 0) uniform sampler2D palette;
layout (set = 1, binding = 1) uniform PaletteUBO {
    uvec2 extent;
    uint size;
    float interval;
    double offset;
    uint smoothing;
    float animationSpeed;
} palette_attr;


layout (set = 2, binding = 0) uniform TimeUBO {
    float time;
} time_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

vec4 getColor(double iteration) {

    if (iteration == 0 || iteration >= iteration_attr.max_value) {
        return vec4(0, 0, 0, 1);
    }
    switch (palette_attr.smoothing) {
        case NONE:
            iteration = iteration - mod(iteration, 1);
            break;
        case NORMAL:
            break;
        case REVERSED:
            iteration = iteration + 1 - 2 * mod(iteration, 1);
            break;
    }

    float hSquare = palette_attr.extent.y - 1;
    float hRemainder = (palette_attr.size - palette_attr.extent.x * hSquare) / palette_attr.extent.x;
    float hLength = hSquare + hRemainder;

    double timedOffsetRatio = palette_attr.offset - double(time_attr.time) * palette_attr.animationSpeed / palette_attr.interval;
    double paletteOffset = mod(iteration / palette_attr.interval + timedOffsetRatio, 1) * hLength;

    float ox = float(mod(paletteOffset, 1));
    float oy = float((floor(paletteOffset) + 0.5) / palette_attr.extent.y);

    return texture(palette, vec2(ox, oy));
}

double getIteration(uvec2 iterCoord) {
    iterCoord.y = iteration_attr.extent.y - iterCoord.y;
    return iteration_attr.iterations[iterCoord.y * iteration_attr.extent.x + iterCoord.x];
}

void main() {

    uvec2 iterCoord = uvec2(gl_FragCoord.xy);

    float x = iterCoord.x;
    float y = iterCoord.y;

    double iteration = getIteration(iterCoord);

    if (iteration == 0) {
        discard;
    }


    color = getColor(iteration);
}