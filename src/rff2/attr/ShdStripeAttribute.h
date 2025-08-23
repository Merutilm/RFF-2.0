#pragma once
#include "StripeType.h"


namespace merutilm::rff2 {
    struct ShdStripeAttribute {
        StripeType stripeType;
        float firstInterval;
        float secondInterval;
        float opacity;
        float offset;
        float animationSpeed;
    };
}