//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererAntialiasing.h"

GLRendererAntialiasing::GLRendererAntialiasing() : GLRenderer("antialiasing"){

}


void GLRendererAntialiasing::setUse(const bool use) {
    this->use = use;
}


void GLRendererAntialiasing::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.uploadBool("use", use);
}

