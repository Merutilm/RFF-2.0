//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "GLRenderer.h"

namespace merutilm::rff2 {
    class GLRendererBoxBlur final : public GLRenderer {
        std::function<void(GLShader &)> additionalParams = [](GLShader &) {
        };

    public:
        explicit GLRendererBoxBlur(std::string_view name);

        ~GLRendererBoxBlur() override = default;

        GLRendererBoxBlur(const GLRendererBoxBlur &) = delete;

        GLRendererBoxBlur &operator=(const GLRendererBoxBlur &) = delete;

        GLRendererBoxBlur(GLRendererBoxBlur &&) = delete;

        GLRendererBoxBlur &operator=(GLRendererBoxBlur &&) = delete;

        void setAdditionalParams(const std::function<void(GLShader &)> &additionalParams);

        void update() override;
    };
}
