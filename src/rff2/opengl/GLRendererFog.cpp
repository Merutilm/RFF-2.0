//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererFog.h"


namespace merutilm::rff2 {
    GLRendererFog::GLRendererFog() : GLRendererGaussianBlur("fog.frag", "gaussian_blur_single_pass.frag") {
    }


    void GLRendererFog::setFogSettings(const ShdFogAttribute &fogSettings) {
        this->fogSettings = &fogSettings;
        setAdditionalBlurParams([this](const GLShader &blurShader) {
            blurShader.uploadFloat("radius", this->fogSettings->radius);
        });
    }


    void GLRendererFog::update() {
        const GLShader &shader = getShader();
        shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
        shader.uploadTexture2D("blurred", GL_TEXTURE1, getBlurredTextureID());
        shader.upload2i("resolution", getWidth(), getHeight());
        shader.uploadFloat("opacity", fogSettings->opacity);
    }
}