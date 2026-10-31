#include <desc_iteration.glsl>

#ifndef UTILS_ITERATION_INCLUDE
#define UTILS_ITERATION_INCLUDE



uint get_iteration_index(ivec2 iter_coord, ivec2 offset) {
    iter_coord.y = int(iteration_info_settings.extent.y) - iter_coord.y - 1;
    iter_coord += offset;
    iter_coord = clamp(iter_coord.xy, ivec2(0), ivec2(iteration_info_settings.extent.xy) - ivec2(1));
    return iter_coord.y * iteration_info_settings.extent.x + iter_coord.x;
}

uint get_iteration_index(ivec2 iter_coord) {
    return get_iteration_index(iter_coord, ivec2(0));
}

double get_iteration(ivec2 iter_coord, ivec2 offset){
    return iteration_settings.iterations[get_iteration_index(iter_coord, offset)];
}

double get_iteration_interpolated(vec2 iter_coord) {
    ivec2 it = ivec2(iter_coord);
    vec2 itd = fract(iter_coord);
    double it_max = iteration_info_settings.max_value;
    double it00 = get_iteration(it, ivec2(0, 0));
    double it01 = get_iteration(it, ivec2(0, 1));
    double it10 = get_iteration(it, ivec2(1, 0));
    double it11 = get_iteration(it, ivec2(1, 1));

    it00 = it_max <= it00 ? it_max * 2 : it00;
    it01 = it_max <= it01 ? it_max * 2 : it01;
    it10 = it_max <= it10 ? it_max * 2 : it10;
    it11 = it_max <= it11 ? it_max * 2 : it11;

    double it0 = it00 * (1 - itd.x) + it10 * itd.x;
    double it1 = it01 * (1 - itd.x) + it11 * itd.x;
    return min(it_max, it0 * itd.y + it1 * (1 - itd.y));
}

double get_iteration(ivec2 iter_coord) {
    return iteration_settings.iterations[get_iteration_index(iter_coord, ivec2(0))];
}


#endif