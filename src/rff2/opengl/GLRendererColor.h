//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRenderer.h"
#include "../attr/ShdColorAttribute.h"
namespace merutilm::rff2 {
    class GLRendererColor final : public GLRenderer {

        const ShdColorAttribute *colorSettings = nullptr;

    public:
        explicit GLRendererColor();

        ~GLRendererColor() override = default;

        GLRendererColor(const GLRendererColor &) = delete;

        GLRendererColor &operator=(const GLRendererColor &) = delete;

        GLRendererColor(GLRendererColor &&) = delete;

        GLRendererColor &operator=(GLRendererColor &&) = delete;

        void setColorSettings(const ShdColorAttribute &colorSettings);

        void update() override;

    };
}