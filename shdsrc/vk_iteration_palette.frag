#version 450

#define ISM_LINEAR 0
#define ISM_SQUARE_ROOT 1
#define ISM_LOG 2

#define SISM_NONE 0
#define SISM_NORMAL 1
#define SISM_REVERSED 2

layout (set = 0, binding = 0) uniform IterUBO {
    uvec2 extent;
    double max_value;
} iteration_info_settings;

layout (set = 0, binding = 1) buffer IterSSBO {
    double iterations[];
} iteration_settings;

layout (set = 1, binding = 0) buffer PaletteSSBO {
    uint size;
    float interval;
    double offset;
    uint smoothing;
    uint single_smoothing;
    float animation_speed;
    vec4 palette[];
} palette_settings;


layout (set = 2, binding = 0) uniform TimeUBO {
    float time;
} time_settings;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

vec4 get_color(double iteration) {

    if (iteration >= iteration_info_settings.max_value) {
        return vec4(0, 0, 0, 1);
    }

    switch (palette_settings.smoothing) {
        case ISM_LINEAR:
            break;
        case ISM_SQUARE_ROOT:
            iteration = sqrt(iteration);
            break;
        case ISM_LOG:
            iteration = log(float(iteration));
            break;
    }

    switch (palette_settings.single_smoothing) {
        case SISM_NONE:
            iteration = iteration - mod(iteration, 1);
            break;
        case SISM_NORMAL:
            break;
        case SISM_REVERSED:
            iteration = iteration + 1 - 2 * mod(iteration, 1);
            break;
    }


    double timed_offset_ratio = palette_settings.offset - double(time_settings.time * palette_settings.animation_speed / palette_settings.interval);
    double palette_offset_ratio = mod(iteration / double(palette_settings.interval) + timed_offset_ratio, 1);
    double palette_offset = palette_offset_ratio * double(palette_settings.size);
    float palette_offset_decimal = float(mod(palette_offset, 1));

    uint cpl = uint(palette_offset_ratio * palette_settings.size);
    uint npl = (cpl + 1) % palette_settings.size;

    vec4 cc = palette_settings.palette[cpl];
    vec4 nc = palette_settings.palette[npl];

    return cc * (1 - palette_offset_decimal) + nc * (palette_offset_decimal);
}

double get_iteration(uvec2 iterCoord) {
    iterCoord.y = iteration_info_settings.extent.y - iterCoord.y - 1;
    return iteration_settings.iterations[iterCoord.y * iteration_info_settings.extent.x + iterCoord.x];
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