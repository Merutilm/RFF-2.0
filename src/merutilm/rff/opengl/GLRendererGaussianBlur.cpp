//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererGaussianBlur.h"


GLRendererGaussianBlur::GLRendererGaussianBlur(const std::string_view name, const std::string_view blurName) : GLRenderer(name) {
    for (int i = 0; i < boxBlurs.size(); ++i) {
        boxBlurs[i] = std::make_unique<GLRendererBoxBlur>(blurName);
    }
}

void GLRendererGaussianBlur::reloadSize(const int w, const int h) {
    for (const auto &boxBlur: boxBlurs) {
        boxBlur->reloadSize(std::min(RFF::Render::GAUSSIAN_MAX_WIDTH * w / h, w),
                           std::min(RFF::Render::GAUSSIAN_MAX_WIDTH, h));
    }
    GLRenderer::reloadSize(w, h);
}

void GLRendererGaussianBlur::setAdditionalBlurParams(std::function<void(GLShaderLoader&)> &&additionalParams) const {
    const std::function<void(GLShaderLoader&)> v = std::move(additionalParams);
    for (const auto &boxBlur: boxBlurs) {
        boxBlur->setAdditionalParams(v);
    }
}


GLuint GLRendererGaussianBlur::getBlurredTextureID() const {
    return boxBlurs.back()->getFBOTextureID();
}


void GLRendererGaussianBlur::setPreviousFBOTextureID(GLuint previousFBOTextureID) {
    GLRenderer::setPreviousFBOTextureID(previousFBOTextureID);
    for (int i = 0; i < RFF::Render::GAUSSIAN_REQUIRES_BOX; ++i) {
        if (i == 0) {
            boxBlurs[i]->setPreviousFBOTextureID(previousFBOTextureID);
        } else {
            boxBlurs[i]->setPreviousFBOTextureID(boxBlurs[i - 1]->getFBOTextureID());
        }
    }
}


void GLRendererGaussianBlur::beforeUpdate() {
    for (auto &boxBlur: boxBlurs) {
        boxBlur->render();
    }
    GLRenderer::beforeUpdate();
}



