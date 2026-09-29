#pragma once
#include "ExploreSettings.hpp"
#include "FileSettings.hpp"
#include "FractalSettings.h"
#include "RenderSettings.h"
#include "ShaderSettings.h"
#include "VideoSettings.h"

namespace merutilm::rff2 {
    struct Settings final{
        FileSettings file;
        FractalSettings fractal;
        RenderSettings render;
        ShaderSettings shader;
        VideoSettings video;
        ExploreSettings explore;
    };
}