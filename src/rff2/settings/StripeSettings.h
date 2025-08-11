#pragma once
#include "StripeType.h"


namespace merutilm::rff2 {
    struct StripeSettings {
        StripeType stripeType;
        float firstInterval;
        float secondInterval;
        float opacity;
        float offset;
        float animationSpeed;
    };
}