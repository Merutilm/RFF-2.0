//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "glad.h"
namespace merutilm::rff {
    struct GLIterationTextureRenderer {
        virtual ~GLIterationTextureRenderer() = default;
        virtual void setIterationTextureID(GLuint textureID) = 0;
    };
}