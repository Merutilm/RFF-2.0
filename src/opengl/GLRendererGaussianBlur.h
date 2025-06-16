//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <memory>

#include "GLRenderer.h"
#include "GLRendererBoxBlur.h"

namespace merutilm::rff {
    class GLRendererGaussianBlur : public GLRenderer {
        std::array<std::unique_ptr<GLRendererBoxBlur>, Constants::Render::GAUSSIAN_REQUIRES_BOX> boxBlurs;
    public:

        explicit GLRendererGaussianBlur(std::string_view name, std::string_view blurName);

        ~GLRendererGaussianBlur() override = default;

        GLRendererGaussianBlur(const GLRendererGaussianBlur &) = delete;

        GLRendererGaussianBlur(GLRendererGaussianBlur &&) = delete;

        GLRendererGaussianBlur &operator=(const GLRendererGaussianBlur &) = delete;

        GLRendererGaussianBlur &operator=(GLRendererGaussianBlur &&) = delete;


        void reloadSize(uint16_t w, uint16_t h) override;

        void setAdditionalBlurParams(std::function<void(GLShader&)> &&additionalParams) const;

        GLuint getBlurredTextureID() const;

        void setPreviousFBOTextureID(GLuint previousFBOTextureID) override;

        void beforeUpdate() override;

        void update() override = 0;
    };
}