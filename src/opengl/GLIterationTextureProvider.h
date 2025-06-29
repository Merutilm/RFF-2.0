//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "glad.h"

namespace merutilm::rff2 {
    struct GLIterationTextureProvider {
        virtual ~GLIterationTextureProvider() = default;
        virtual GLuint getIterationTextureID() = 0;
    };
}