//
// Created by Merutilm on 2025-05-28.
//

#include "StripePresets.h"

std::string merutilm::rff::StripePresets::SlowAnimated::getName() const {
    return "Slow Animated";
}

merutilm::rff::StripeSettings merutilm::rff::StripePresets::SlowAnimated::stripeSettings() const {
    return StripeSettings{StripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f};
}
