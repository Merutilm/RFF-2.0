//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <memory>

#include "GLRenderer.h"
#include "GLRendererBoxBlur.h"

class GLRendererGaussianBlur : public GLRenderer {
    std::array<std::unique_ptr<GLRendererBoxBlur>, RFF::Render::GAUSSIAN_REQUIRES_BOX> boxBlurs;
public:

    explicit GLRendererGaussianBlur(std::string_view name, std::string_view blurName);

    ~GLRendererGaussianBlur() override = default;

    GLRendererGaussianBlur(const GLRendererGaussianBlur &) = delete;

    GLRendererGaussianBlur(GLRendererGaussianBlur &&) = delete;

    GLRendererGaussianBlur &operator=(const GLRendererGaussianBlur &) = delete;

    GLRendererGaussianBlur &operator=(GLRendererGaussianBlur &&) = delete;


    void reloadSize(int w, int h) override;

    void setAdditionalBlurParams(std::function<void(GLShaderLoader&)> &&additionalParams) const;

    GLuint getBlurredTextureID() const;

    void setPreviousFBOTextureID(GLuint previousFBOTextureID) override;

    void beforeUpdate() override;

    void update() override = 0;
};
