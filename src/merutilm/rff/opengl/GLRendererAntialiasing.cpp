//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererAntialiasing.h"

merutilm::rff::GLRendererAntialiasing::GLRendererAntialiasing() : GLRenderer("antialiasing.frag"){

}


void merutilm::rff::GLRendererAntialiasing::setUse(const bool use) {
    this->use = use;
}


void merutilm::rff::GLRendererAntialiasing::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.uploadBool("use", use);
}

