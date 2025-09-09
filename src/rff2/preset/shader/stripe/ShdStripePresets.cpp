//
// Created by Merutilm on 2025-05-28.
//

#include "ShdStripePresets.h"

namespace merutilm::rff2 {

    std::string ShdStripePresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdStripeAttribute ShdStripePresets::Disabled::genStripe() const {
        return ShdStripeAttribute{ShdStripeType::NONE, 10, 50, 1, 0, 0};
    }

    std::string ShdStripePresets::SlowAnimated::getName() const {
        return "Slow Animated";
    }

    ShdStripeAttribute ShdStripePresets::SlowAnimated::genStripe() const {
        return ShdStripeAttribute{ShdStripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f};
    }

    std::string ShdStripePresets::FastAnimated::getName() const {
        return "Fast Animated";
    }

    ShdStripeAttribute ShdStripePresets::FastAnimated::genStripe() const {
        return ShdStripeAttribute{ShdStripeType::SINGLE_DIRECTION, 100, 500, 1, 0, 5};
    }

    std::string ShdStripePresets::Smooth::getName() const {
        return "Smooth";
    }

    ShdStripeAttribute ShdStripePresets::Smooth::genStripe() const {
        return ShdStripeAttribute{ShdStripeType::SMOOTH, 1, 1, 1, 0, 0.25f};
    }

    std::string ShdStripePresets::SmoothTranslucent::getName() const {
        return "Smooth Translucent";
    }

    ShdStripeAttribute ShdStripePresets::SmoothTranslucent::genStripe() const {
        return ShdStripeAttribute{ShdStripeType::SQUARED, 1, 1, 0.5f, 0, 1};
    }
}
