//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLIterationTextureRenderer.h"
#include "GLRenderer.h"
#include "../settings/SlopeSettings.h"

class GLRendererSlope final : public GLRenderer, public GLIterationTextureRenderer{
    int iterationTextureID = 0;
    float clarityMultiplier = 1;
    const SlopeSettings *slopeSettings = nullptr;

public:
    explicit GLRendererSlope();

    ~GLRendererSlope() override = default;

    GLRendererSlope(const GLRendererSlope&) = delete;

    GLRendererSlope(GLRendererSlope&) = delete;

    GLRendererSlope& operator=(const GLRendererSlope&) = delete;

    GLRendererSlope(GLRendererSlope&&) = delete;

    GLRendererSlope& operator=(GLRendererSlope&&) = delete;

    void setIterationTextureID(GLuint textureID) override;

    void setClarityMultiplier(float multiplier) override;

    void setSlopeSettings(const SlopeSettings &slopeSettings);

    void update() override;

};
