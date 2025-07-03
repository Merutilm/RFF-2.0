//
// Created by Merutilm on 2025-05-28.
//

#include "StripePresets.h"

namespace merutilm::rff2 {

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

    std::string StripePresets::FastAnimated::getName() const {
        return "Fast Animated";
    }

    StripeSettings StripePresets::FastAnimated::stripeSettings() const {
        return StripeSettings{StripeType::SINGLE_DIRECTION, 100, 500, 1, 0, 5};
    }

    std::string StripePresets::Smooth::getName() const {
        return "Smooth";
    }

    StripeSettings StripePresets::Smooth::stripeSettings() const {
        return StripeSettings{StripeType::SMOOTH, 1, 1, 1, 0, 0.25f};
    }

    std::string StripePresets::SmoothTranslucent::getName() const {
        return "Smooth Translucent";
    }

    StripeSettings StripePresets::SmoothTranslucent::stripeSettings() const {
        return StripeSettings{StripeType::SQUARED, 20, 100, 0.5f, 0, 1};
    }
}
