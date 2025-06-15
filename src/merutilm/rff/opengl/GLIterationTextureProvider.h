//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "glad.h"

namespace merutilm::rff {
    struct GLIterationTextureProvider {
        virtual ~GLIterationTextureProvider() = default;
        virtual GLuint getIterationTextureID() = 0;
    };
}