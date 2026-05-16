//
// Created by Merutilm on 2025-05-28.
//

#include "ShdStripePresets.h"

namespace merutilm::rff2 {

    std::wstring ShdStripePresets::Disabled::getName() const {
        return L"Disabled";
    }

    ShdStripeSettings ShdStripePresets::Disabled::genStripe() const {
        return ShdStripeSettings{ShdStripeType::NONE, 10, 50, 1, 0, 0, ShdIterationColoringMethod::LINEAR};
    }

    std::wstring ShdStripePresets::SlowAnimated::getName() const {
        return L"Slow Animated";
    }

    ShdStripeSettings ShdStripePresets::SlowAnimated::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SINGLE_DIRECTION, 10, 50, 1, 0, 0.5f, ShdIterationColoringMethod::LINEAR};
    }

    std::wstring ShdStripePresets::FastAnimated::getName() const {
        return L"Fast Animated";
    }

    ShdStripeSettings ShdStripePresets::FastAnimated::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SINGLE_DIRECTION, 100, 500, 1, 0, 5, ShdIterationColoringMethod::LINEAR};
    }

    std::wstring ShdStripePresets::Smooth::getName() const {
        return L"Smooth";
    }

    ShdStripeSettings ShdStripePresets::Smooth::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SMOOTH, 1, 1, 1, 0, 0.25f, ShdIterationColoringMethod::LINEAR};
    }

    std::wstring ShdStripePresets::SmoothTranslucent::getName() const {
        return L"Smooth Translucent";
    }

    ShdStripeSettings ShdStripePresets::SmoothTranslucent::genStripe() const {
        return ShdStripeSettings{ShdStripeType::SQUARED, 1, 1, 0.5f, 0, 1, ShdIterationColoringMethod::LINEAR};
    }
}
