//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererColor.h"


GLRendererColor::GLRendererColor() : GLRenderer("color") {

}


void GLRendererColor::setColorSettings(const ColorSettings &colorSettings) {
    this->colorSettings = &colorSettings;
}



void GLRendererColor::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.uploadFloat("gamma", colorSettings->gamma);
    shader.uploadFloat("exposure", colorSettings->exposure);
    shader.uploadFloat("hue", colorSettings->hue);
    shader.uploadFloat("saturation", colorSettings->saturation);
    shader.uploadFloat("brightness", colorSettings->brightness);
    shader.uploadFloat("contrast", colorSettings->contrast);

}


