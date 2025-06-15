//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererBloom.h"

merutilm::rff::GLRendererBloom::GLRendererBloom() : GLRendererGaussianBlur("bloom.frag", "gaussian_blur_for_bloom_single_pass.frag") {
}

void merutilm::rff::GLRendererBloom::setBloomSettings(const BloomSettings &bloomSettings) {
    this->bloomSettings = &bloomSettings;
    setAdditionalBlurParams([this](const GLShaderLoader &blurShader) {
            blurShader.uploadTexture2D("original", GL_TEXTURE2, getPrevFBOTextureID());
            blurShader.uploadFloat("radius", this->bloomSettings->radius);
            blurShader.uploadFloat("threshold", this->bloomSettings->threshold);
        });
}

void merutilm::rff::GLRendererBloom::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.uploadTexture2D("blurred", GL_TEXTURE1, getBlurredTextureID());
    shader.upload2i("resolution", getWidth(), getHeight());
    shader.uploadFloat("softness", bloomSettings->softness);
    shader.uploadFloat("intensity", bloomSettings->intensity);
}

