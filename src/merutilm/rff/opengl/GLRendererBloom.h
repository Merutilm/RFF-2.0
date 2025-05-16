//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRendererGaussianBlur.h"
#include "../settings/BloomSettings.h"

class GLRendererBloom final : public GLRendererGaussianBlur {
    const BloomSettings *bloomSettings = nullptr;

public:
    explicit GLRendererBloom();

    ~GLRendererBloom() override = default;

    GLRendererBloom(const GLRendererBloom &) = delete;

    GLRendererBloom &operator=(const GLRendererBloom &) = delete;

    GLRendererBloom(GLRendererBloom &&) = delete;

    GLRendererBloom &operator=(GLRendererBloom &&) = delete;

    void setBloomSettings(const BloomSettings &bloomSettings);

    void update() override;

};
