//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererGaussianBlur.h"


namespace merutilm::rff {
    GLRendererGaussianBlur::GLRendererGaussianBlur(const std::string_view name, const std::string_view blurName) : GLRenderer(name) {
        for (int i = 0; i < boxBlurs.size(); ++i) {
            boxBlurs[i] = std::make_unique<GLRendererBoxBlur>(blurName);
        }
    }

    void GLRendererGaussianBlur::reloadSize(const uint16_t w, const uint16_t h) {
        for (const auto &boxBlur: boxBlurs) {
            boxBlur->reloadSize(std::min(static_cast<uint16_t>(static_cast<uint32_t>(Constants::Render::GAUSSIAN_MAX_WIDTH) * w / h), w),
                                std::min(Constants::Render::GAUSSIAN_MAX_WIDTH, h));
        }
        GLRenderer::reloadSize(w, h);
    }

    void GLRendererGaussianBlur::setAdditionalBlurParams(std::function<void(GLShader&)> &&additionalParams) const {
        const std::function<void(GLShader&)> v = std::move(additionalParams);
        for (const auto &boxBlur: boxBlurs) {
            boxBlur->setAdditionalParams(v);
        }
    }


    GLuint GLRendererGaussianBlur::getBlurredTextureID() const {
        return boxBlurs.back()->getFBOTextureID();
    }


    void GLRendererGaussianBlur::setPreviousFBOTextureID(const GLuint previousFBOTextureID) {
        GLRenderer::setPreviousFBOTextureID(previousFBOTextureID);
        for (int i = 0; i < Constants::Render::GAUSSIAN_REQUIRES_BOX; ++i) {
            if (i == 0) {
                boxBlurs[i]->setPreviousFBOTextureID(previousFBOTextureID);
            } else {
                boxBlurs[i]->setPreviousFBOTextureID(boxBlurs[i - 1]->getFBOTextureID());
            }
        }
    }


    void GLRendererGaussianBlur::beforeUpdate() {
        for (const auto &boxBlur: boxBlurs) {
            boxBlur->render();
        }
        GLRenderer::beforeUpdate();
    }
}


