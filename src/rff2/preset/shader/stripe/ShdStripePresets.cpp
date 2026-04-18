//
// Created by Merutilm on 2025-05-28.
//

#include "ShdStripePresets.h"

namespace merutilm::rff2 {

    std::string ShdStripePresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdStripeSettings ShdStripePresets::Disabled::genStripe() const {
        return ShdStripeSettings{ShdStripeType::NONE, 10, 50, 1, 0, 0};
    }

    std::string ShdStripePresets::SlowAnimated::getName() const {
        return "Slow Animated";
    }

    ShdStripeSettings ShdStripePresets::SlowAnimated::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f};
    }

    std::string ShdStripePresets::FastAnimated::getName() const {
        return "Fast Animated";
    }

    ShdStripeSettings ShdStripePresets::FastAnimated::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SINGLE_DIRECTION, 100, 500, 1, 0, 5};
    }

    std::string ShdStripePresets::Smooth::getName() const {
        return "Smooth";
    }

    ShdStripeSettings ShdStripePresets::Smooth::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SMOOTH, 1, 1, 1, 0, 0.25f};
    }

    std::string ShdStripePresets::SmoothTranslucent::getName() const {
        return "Smooth Translucent";
    }

    ShdStripeSettings ShdStripePresets::SmoothTranslucent::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SQUARED, 1, 1, 0.5f, 0, 1};
    }
}
