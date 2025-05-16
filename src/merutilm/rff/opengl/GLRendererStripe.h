//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLIterationTextureRenderer.h"
#include "GLRenderer.h"
#include "GLTimeRenderer.h"
#include "../settings/StripeSettings.h"

class GLRendererStripe final : public GLRenderer, public GLIterationTextureRenderer, public GLTimeRenderer {
    GLuint iterationTextureID = 0;
    float clarityMultiplier = 1;
    const StripeSettings *stripeSettings = nullptr;
    float time = 0;

public:
    explicit GLRendererStripe();

    ~GLRendererStripe() override = default;

    GLRendererStripe(GLRendererStripe const &) = delete;

    GLRendererStripe &operator=(GLRendererStripe const &) = delete;

    GLRendererStripe(GLRendererStripe &&) = delete;

    GLRendererStripe &operator=(GLRendererStripe &&) = delete;

    void setIterationTextureID(GLuint textureID) override;

    void setClarityMultiplier(float multiplier) override;

    void setTime(float time) override;

    void setStripeSettings(const StripeSettings &stripeSettings);

    void update() override;
};
