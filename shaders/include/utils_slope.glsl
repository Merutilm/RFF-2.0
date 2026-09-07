#include <desc_slope.glsl>
#include <desc_iteration.glsl>
#include <utils_iteration.glsl>

#ifndef UTILS_SLOPE_INCLUDE
#define UTILS_SLOPE_INCLUDE

float slope_get_shade(double ld, double d, double rd, double l, double r, double lu, double u, double ru, ivec2 iter_coord, float m) {

    if(slope_settings.reflection_ratio >= 1 || slope_settings.depth == 0){
        return 1;
    }

    float multiplier = float(iteration_info_settings.extent.x) / 1280 * m;

    float aRad = radians(slope_settings.azimuth);
    float zRad = radians(slope_settings.zenith);
    float dzDx = float((rd + 2 * r + ru) - (ld + 2 * l + lu)) * slope_settings.depth * multiplier;
    float dzDy = float((lu + 2 * u + ru) - (ld + 2 * d + rd)) * slope_settings.depth * multiplier;
    float slope = atan(length(vec2(dzDx, dzDy)));
    float aspect = atan(dzDy, -dzDx);
    float shade = max(slope_settings.reflection_ratio, cos(zRad) * cos(slope) + sin(zRad) * sin(slope) * cos(aRad + aspect));
    return 1 - slope_settings.opacity * (1 - shade);
}

float slope_get_shade(ivec2 iter_coord) {
    double ld = get_iteration(iter_coord, ivec2(-1, -1));
    double d = get_iteration(iter_coord, ivec2(0, -1));
    double rd = get_iteration(iter_coord, ivec2(1, -1));
    double l = get_iteration(iter_coord, ivec2(-1, 0));
    double r = get_iteration(iter_coord, ivec2(1, 0));
    double lu = get_iteration(iter_coord, ivec2(-1, 1));
    double u = get_iteration(iter_coord, ivec2(0, 1));
    double ru = get_iteration(iter_coord, ivec2(1, 1));
    return slope_get_shade(ld, d, rd, l, r, lu, u, ru, iter_coord, 1);
}


#endif