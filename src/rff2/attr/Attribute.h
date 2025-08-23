#pragma once
#include "CalcAttribute.h"
#include "RenderAttribute.h"
#include "ShaderAttribute.h"
#include "VideoAttribute.h"

namespace merutilm::rff2 {
    struct Attribute final{
        CalcAttribute calc;
        RenderAttribute render;
        ShaderAttribute shader;
        VideoAttribute video;

    };
}