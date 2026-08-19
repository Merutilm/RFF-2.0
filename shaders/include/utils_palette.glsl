#include <desc_time.glsl>
#include <desc_iteration.glsl>
#include <desc_palette.glsl>
#include <enums.glsl>

#ifndef UTILS_PALETTE_INCLUDE
#define UTILS_PALETTE_INCLUDE

vec4 palette_get_color(double iteration) {

    if (iteration == 0 || iteration >= iteration_info_settings.max_value) {
        return vec4(0, 0, 0, 1);
    }

    switch (palette_settings.coloring) {
        case ICM_LINEAR:
            break;
        case ICM_SQUARE_ROOT:
            iteration = sqrt(iteration);
            break;
        case ICM_LOG:
            iteration = log(float(iteration));
            break;
    }

    switch (palette_settings.single_coloring) {
        case SICM_NONE:
            iteration = iteration - mod(iteration, 1);
            break;
        case SICM_NORMAL:
            break;
        case SICM_REVERSED:
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

#endif