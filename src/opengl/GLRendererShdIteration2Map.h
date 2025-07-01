//
// Created by Merutilm on 2025-07-01.
//

#pragma once
#include "GLRendererIteration2Map.h"

namespace merutilm::rff2 {
    struct GLRendererShdIteration2Map final : public GLRendererIteration2Map, public GLIterationTextureProvider{
        GLRendererShdIteration2Map() : GLRendererIteration2Map("shd_iteration_palette_2_map.frag") {};

        GLuint getIterationTextureID() override {return getFBOTextureID();};
    };
}
