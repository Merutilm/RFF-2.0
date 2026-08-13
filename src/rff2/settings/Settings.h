#pragma once
#include "ExploreSettings.hpp"
#include "FractalSettings.h"
#include "RenderSettings.h"
#include "ShaderSettings.h"
#include "VideoSettings.h"

namespace merutilm::rff2 {
    struct Settings final{
        FractalSettings fractal;
        RenderSettings render;
        ShaderSettings shader;
        VideoSettings video;
        ExploreSettings explore;
    };
}