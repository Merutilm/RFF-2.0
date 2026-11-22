#include <utils_complex.glsl>

#ifndef UTILS_PERTURBATION_INCLUDE
#define UTILS_PERTURBATION_INCLUDE

#define BAILOUT_THRESHOLD_ITERATION_SMOOTHING_METHOD 64

FexComplex offset_conversion(uvec2 pixel, uvec2 extent) {
    return fex_complex_div(fex_complex((vec2(pixel) - vec2(extent) / 2) / render_meta.clarity_multiplier), fex_exp10(render_meta.log_zoom));
}


uint compress(uint64_t ref_iteration) {
    //TODO compress
    return uint(ref_iteration);
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

    if (prev_distance == curr_distance || render_meta.smooth_iteration == 0) {
        return 0.0;
    }
    float bailout = render_meta.bailout;

    return bailout >= BAILOUT_THRESHOLD_ITERATION_SMOOTHING_METHOD
    ? 1 - log(log(curr_distance) / LN2) / LN2
    : log(bailout * (bailout - prev_distance) / (curr_distance - prev_distance) + 1) / log(bailout + 1);

}

#endif