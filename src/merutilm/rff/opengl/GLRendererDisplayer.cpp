//
// Created by Merutilm on 2025-05-28.
//

#include "GLRendererDisplayer.h"

GLRendererDisplayer::GLRendererDisplayer() : GLRenderer("displayer") {
    setToDisplay();
}

void GLRendererDisplayer::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.upload2i("resolution", getWidth(), getHeight());
}

