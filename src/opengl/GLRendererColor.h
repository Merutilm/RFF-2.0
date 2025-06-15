//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRenderer.h"
#include "../settings/ColorSettings.h"
namespace merutilm::rff {
    class GLRendererColor final : public GLRenderer {

        const ColorSettings *colorSettings = nullptr;

    public:
        explicit GLRendererColor();

        ~GLRendererColor() override = default;

        GLRendererColor(const GLRendererColor &) = delete;

        GLRendererColor &operator=(const GLRendererColor &) = delete;

        GLRendererColor(GLRendererColor &&) = delete;

        GLRendererColor &operator=(GLRendererColor &&) = delete;

        void setColorSettings(const ColorSettings &colorSettings);

        void update() override;

    };
}