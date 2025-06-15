#pragma once
#include "CalculationSettings.h"
#include "RenderSettings.h"
#include "ShaderSettings.h"
#include "VideoSettings.h"

namespace merutilm::rff {
    struct Settings final{
        CalculationSettings calculationSettings;
        RenderSettings renderSettings;
        ShaderSettings shaderSettings;
        VideoSettings videoSettings;

    };
}