//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererAntialiasing.h"

namespace merutilm::rff2 {
    GLRendererAntialiasing::GLRendererAntialiasing() : GLRenderer("antialiasing.frag"){

    }


    void GLRendererAntialiasing::setUse(const bool use) {
        this->use = use;
    }


    void GLRendererAntialiasing::update() {
        const GLShader &shader = getShader();
        shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
        shader.uploadBool("use", use);
    }
}