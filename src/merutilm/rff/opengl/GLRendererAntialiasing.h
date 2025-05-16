//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRenderer.h"

class GLRendererAntialiasing final : public GLRenderer {

    bool use = false;
    public:
    explicit GLRendererAntialiasing();

    ~GLRendererAntialiasing() override = default;

    GLRendererAntialiasing(const GLRendererAntialiasing &) = delete;

    GLRendererAntialiasing &operator=(const GLRendererAntialiasing &) = delete;

    GLRendererAntialiasing(GLRendererAntialiasing &&) = delete;

    GLRendererAntialiasing &operator=(GLRendererAntialiasing &&) = delete;

    void setUse(bool use);

    void update() override;
};
