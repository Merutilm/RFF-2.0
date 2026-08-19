#ifndef DESC_STRIPE_INCLUDE
#define DESC_STRIPE_INCLUDE


layout (set = DESC_STRIPE, binding = 0) uniform StripeUBO {
    uint type;
    float first_interval;
    float second_interval;
    float opacity;
    float offset;
    float animation_speed;
    uint coloring;
} stripe_settings;

#endif