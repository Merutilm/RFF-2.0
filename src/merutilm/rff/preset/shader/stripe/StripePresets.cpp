//
// Created by Merutilm on 2025-05-28.
//

#include "StripePresets.h"

std::string StripePresets::SlowAnimated::getName() {
    return "Slow Animated";
}

StripeSettings StripePresets::SlowAnimated::stripeSettings() {
    return StripeSettings{StripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f};
}
