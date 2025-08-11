#pragma once
#include "CalculationSettings.h"
#include "RenderSettings.h"
#include "ShaderSettings.h"
#include "VideoSettings.h"

namespace merutilm::rff2 {
    struct Settings final{
        CalculationSettings calculationSettings;
        RenderSettings renderSettings;
        ShaderSettings shaderSettings;
        VideoSettings videoSettings;

    };
}