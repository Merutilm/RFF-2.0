//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "glad.h"

struct GLIterationTextureProvider {
    virtual ~GLIterationTextureProvider() = default;
    virtual GLuint getIterationTextureID() = 0;
    virtual float getClarityMultiplier() = 0;
};
