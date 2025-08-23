#pragma once
#include "ShdBloomAttribute.h"
#include "ShdColorAttribute.h"
#include "ShdFogAttribute.h"
#include "ShdPaletteAttribute.h"
#include "ShdSlopeAttribute.h"
#include "ShdStripeAttribute.h"


namespace merutilm::rff2 {
    struct ShaderAttribute {
        ShdPaletteAttribute palette;
        ShdStripeAttribute stripe;
        ShdSlopeAttribute slope;
        ShdColorAttribute color;
        ShdFogAttribute fog;
        ShdBloomAttribute bloom;
    };
}
