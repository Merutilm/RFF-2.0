//
// Created by Merutilm on 2025-05-28.
//

#include "ShdStripePresets.hpp"

namespace merutilm::rff2::ShdStripePresets {

    std::string Disabled::getName() const {
        return "Disabled";
    }

    ShdStripeSettings Disabled::genStripe() const {
        return ShdStripeSettings{ShdStripeType::NONE, 10, 50, 1, 0, 0, ShdIterationColoringMethod::LINEAR};
    }

    std::string SlowAnimated::getName() const {
        return "Slow Animated";
    }

    ShdStripeSettings SlowAnimated::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f, ShdIterationColoringMethod::LINEAR};
    }

    std::string FastAnimated::getName() const {
        return "Fast Animated";
    }

    ShdStripeSettings FastAnimated::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SINGLE_DIRECTION, 100, 500, 1, 0, 5, ShdIterationColoringMethod::LINEAR};
    }

    std::string Smooth::getName() const {
        return "Smooth";
    }

    ShdStripeSettings Smooth::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SMOOTH, 1, 1, 1, 0, 0.25f, ShdIterationColoringMethod::LINEAR};
    }

    std::string SmoothTranslucent::getName() const {
        return "Smooth Translucent";
    }

    ShdStripeSettings SmoothTranslucent::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SQUARED, 1, 1, 0.5f, 0, 1, ShdIterationColoringMethod::LINEAR};
    }
}
