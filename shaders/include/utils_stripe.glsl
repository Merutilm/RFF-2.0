#include <common.glsl>
#include <desc_stripe.glsl>
#include <desc_time.glsl>

#ifndef UTILS_STRIPE_INCLUDE
#define UTILS_STRIPE_INCLUDE

float stripe_get_multiplier(double iteration){

    switch (stripe_settings.coloring) {
        case ICM_LINEAR:
            break;
        case ICM_SQUARE_ROOT:
            iteration = sqrt(iteration);
            break;
        case ICM_LOG:
            iteration = log(float(iteration));
            break;
    }

    double iter_curr = iteration - (stripe_settings.offset + stripe_settings.animation_speed * time_settings.time);
    float black;
    float rat1 = float(mod(iter_curr, stripe_settings.first_interval)) / stripe_settings.first_interval;
    float rat2 = float(mod(iter_curr, stripe_settings.second_interval)) / stripe_settings.second_interval;

    switch (stripe_settings.type) {
        case ST_SINGLE_DIRECTION: {
                                   black = rat1 * rat2;
                                   break;
                               }
        case ST_SMOOTH: {
                                   black = pow((sin(rat1 * DOUBLE_PI) + 1) * (sin(rat2 * DOUBLE_PI) + 1) / 4, 2);
                                   break;
                               }
        case ST_SMOOTH_SQUARED: {
                                   black = pow((sin(rat1 * DOUBLE_PI) + 1) * (sin(rat2 * DOUBLE_PI) + 1) / 4, 4);
                                   break;
                               }
    }
    return 1 - black * stripe_settings.opacity;
}

#endif