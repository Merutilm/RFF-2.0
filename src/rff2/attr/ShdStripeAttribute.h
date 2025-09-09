#pragma once
#include "ShdStripeType.h"


namespace merutilm::rff2 {
    struct ShdStripeAttribute {
        ShdStripeType stripeType;
        float firstInterval;
        float secondInterval;
        float opacity;
        float offset;
        float animationSpeed;
    };
}