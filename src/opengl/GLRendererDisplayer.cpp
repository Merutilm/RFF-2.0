//
// Created by Merutilm on 2025-05-28.
//

#include "GLRendererDisplayer.h"

merutilm::rff::GLRendererDisplayer::GLRendererDisplayer() : GLRenderer("displayer.frag") {
    setToDisplay();
}

void merutilm::rff::GLRendererDisplayer::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.upload2i("resolution", getWidth(), getHeight());
}

