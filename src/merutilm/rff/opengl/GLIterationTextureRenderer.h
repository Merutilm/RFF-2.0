//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "glad.h"

struct GLIterationTextureRenderer {
    virtual ~GLIterationTextureRenderer() = default;
    virtual void setIterationTextureID(GLuint textureID) = 0;
    virtual void setClarityMultiplier(float multiplier) = 0;
};
