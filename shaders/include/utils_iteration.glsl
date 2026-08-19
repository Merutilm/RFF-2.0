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

double get_iteration(ivec2 iter_coord) {
    return iteration_settings.iterations[get_iteration_index(iter_coord, ivec2(0))];
}


#endif