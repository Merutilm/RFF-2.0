//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererFog.h"


merutilm::rff::GLRendererFog::GLRendererFog() : GLRendererGaussianBlur("fog.frag", "gaussian_blur_single_pass.frag") {
}


void merutilm::rff::GLRendererFog::setFogSettings(const FogSettings &fogSettings) {
    this->fogSettings = &fogSettings;
    setAdditionalBlurParams([this](const GLShaderLoader &blurShader) {
        blurShader.uploadFloat("radius", this->fogSettings->radius);
    });
}


void merutilm::rff::GLRendererFog::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.uploadTexture2D("blurred", GL_TEXTURE1, getBlurredTextureID());
    shader.upload2i("resolution", getWidth(), getHeight());
    shader.uploadFloat("opacity", fogSettings->opacity);
}
