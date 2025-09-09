#version 450
#define NONE 0
#define NORMAL 1
#define REVERSED 2

layout (set = 0, binding = 0) uniform IterUBO {
    uvec2 extent;
    double max_value;
} iteration_info_attr;

layout (set = 0, binding = 1) buffer IterSSBO {
    double iterations[];
} iteration_attr;

layout (set = 1, binding = 0) buffer PaletteSSBO {
    uint size;
    float interval;
    double offset;
    uint smoothing;
    float animation_speed;
    vec4 palette[];
} palette_attr;


layout (set = 2, binding = 0) uniform TimeUBO {
    float time;
} time_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

vec4 get_color(double iteration) {

    if (iteration == 0 || iteration >= iteration_info_attr.max_value) {
        discard;
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


    double timed_offset_ratio = palette_attr.offset - double(time_attr.time * palette_attr.animation_speed / palette_attr.interval);
    double palette_offset_ratio = mod(iteration / double(palette_attr.interval) + timed_offset_ratio, 1);
    double palette_offset = palette_offset_ratio * double(palette_attr.size);
    float palette_offset_decimal = float(mod(palette_offset, 1));

    uint cpl = uint(palette_offset_ratio * palette_attr.size);
    uint npl = (cpl + 1) % palette_attr.size;

    vec4 cc = palette_attr.palette[cpl];
    vec4 nc = palette_attr.palette[npl];

    return cc * (1 - palette_offset_decimal) + nc * (palette_offset_decimal);
}

double get_iteration(uvec2 iterCoord) {
    iterCoord.y = iteration_info_attr.extent.y - iterCoord.y;
    return iteration_attr.iterations[iterCoord.y * iteration_info_attr.extent.x + iterCoord.x];
}

void main() {

    uvec2 iter_coord = uvec2(gl_FragCoord.xy);

    float x = iter_coord.x;
    float y = iter_coord.y;

    double iteration = get_iteration(iter_coord);

    if (iteration == 0) {
        color = vec4(0, 0, 0, 1);
        return;
    }

    color = get_color(iteration);
}