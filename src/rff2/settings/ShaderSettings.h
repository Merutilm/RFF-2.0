#pragma once
#include "BloomSettings.h"
#include "ColorSettings.h"
#include "FogSettings.h"
#include "PaletteSettings.h"
#include "SlopeSettings.h"
#include "StripeSettings.h"


namespace merutilm::rff2 {
    struct ShaderSettings {
        PaletteSettings paletteSettings;
        StripeSettings stripeSettings;
        SlopeSettings slopeSettings;
        ColorSettings colorSettings;
        FogSettings fogSettings;
        BloomSettings bloomSettings;
    };
}
