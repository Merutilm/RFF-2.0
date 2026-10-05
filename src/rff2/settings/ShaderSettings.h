#pragma once
#include "ShdBloomSettings.h"
#include "ShdColorSettings.h"
#include "ShdFogSettings.h"
#include "ShdFractal3DSettings.hpp"
#include "ShdNoiseReduction.hpp"
#include "ShdPaletteSettings.h"
#include "ShdStripeSettings.h"
#include "ShdSurfaceSettings.h"


namespace merutilm::rff2 {
    struct ShaderSettings {
        ShdPaletteSettings palette;
        ShdStripeSettings stripe;
        ShdSurfaceSettings surface;
        ShdColorSettings color;
        ShdFogSettings fog;
        ShdBloomSettings bloom;
        ShdNoiseReduction noiseReduction;
        ShdFractal3DSettings fractal3D;
    };
}
