//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererColor.h"


namespace merutilm::rff2 {
    GLRendererColor::GLRendererColor() : GLRenderer("color.frag") {

    }


    void GLRendererColor::setColorSettings(const ShdColorAttribute &colorSettings) {
        this->colorSettings = &colorSettings;
    }



    void GLRendererColor::update() {
        const GLShader &shader = getShader();
        shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
        shader.uploadFloat("gamma", colorSettings->gamma);
        shader.uploadFloat("exposure", colorSettings->exposure);
        shader.uploadFloat("hue", colorSettings->hue);
        shader.uploadFloat("saturation", colorSettings->saturation);
        shader.uploadFloat("brightness", colorSettings->brightness);
        shader.uploadFloat("contrast", colorSettings->contrast);

    }
}

