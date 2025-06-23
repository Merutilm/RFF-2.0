//
// Created by Merutilm on 2025-05-28.
//

#include "StripePresets.h"

namespace merutilm::rff {

    std::string StripePresets::Disabled::getName() const {
        return "Disabled";
    }

    StripeSettings StripePresets::Disabled::stripeSettings() const {
        return StripeSettings{StripeType::NONE, 10, 50, 1, 0, 0};
    }

    std::string StripePresets::SlowAnimated::getName() const {
        return "Slow Animated";
    }

    StripeSettings StripePresets::SlowAnimated::stripeSettings() const {
        return StripeSettings{StripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f};
    }
}