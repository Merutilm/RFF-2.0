#pragma once
#include "ShdStripeType.h"
#include "ShdIterationColoringMethod.hpp"

namespace merutilm::rff2 {
    struct ShdStripeSettings {
        ShdStripeType stripeType;
        float firstInterval;
        float secondInterval;
        float opacity;
        float offset;
        float animationSpeed;
        ShdIterationColoringMethod iterationColoring;
    };
}