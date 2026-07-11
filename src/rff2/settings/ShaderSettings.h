#pragma once
#include "ShdBloomSettings.h"
#include "ShdColorSettings.h"
#include "ShdFogSettings.h"
#include "ShdPaletteSettings.h"
#include "ShdSlopeSettings.h"
#include "ShdStripeSettings.h"


namespace merutilm::rff2 {
    struct ShaderSettings {
        ShdPaletteSettings palette;
        ShdStripeSettings stripe;
        ShdSlopeSettings slope;
        ShdColorSettings color;
        ShdFogSettings fog;
        ShdBloomSettings bloom;
    };
}
