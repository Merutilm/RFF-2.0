//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLIterationTextureRenderer.h"
#include "GLRenderer.h"
#include "../settings/SlopeSettings.h"

namespace merutilm::rff {
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

        void setSlopeSettings(const SlopeSettings &slopeSettings);

        void setClarityMultiplier(float clarityMultiplier);

        void update() override;

    };
}