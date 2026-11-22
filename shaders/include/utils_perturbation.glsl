#include <utils_complex.glsl>

#ifndef UTILS_PERTURBATION_INCLUDE
#define UTILS_PERTURBATION_INCLUDE


FexComplex offset_conversion(uvec2 pixel, uvec2 extent) {
    return fex_complex_div(fex_complex((vec2(pixel) - vec2(extent) / 2) / render_meta.clarity_multiplier), fex_exp10(render_meta.log_zoom));
}


uint compress(uint64_t ref_iteration) {
    //TODO compress
    return uint(ref_iteration);
}


float apply_decimalize(float ratio) {
    if(render_meta.smooth_iteration == 0) return 0;
    float p = render_meta.bailout;
    return log(p * ratio + 1) / log(p + 1);
}

float get_exterior_double_value_iteration_ratio(float prev_distance, float curr_distance) {
    // prevIterDistance = p
    // currIterDistance = c
    // bailout = b
    //
    // a = b - p (p < b)
    // b = c - b (c > b)
    // 0 dec 1 decimal value
    // a : b ratio
    // ratio = a / (a + b) = (b - p) / (c - p)

    if (prev_distance == curr_distance) {
        return 0.0;
    }
    return apply_decimalize((render_meta.bailout - prev_distance) / (curr_distance - prev_distance));
}

#endif