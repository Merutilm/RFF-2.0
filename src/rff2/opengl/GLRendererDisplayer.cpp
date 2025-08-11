//
// Created by Merutilm on 2025-05-28.
//

#include "GLRendererDisplayer.h"


namespace merutilm::rff2 {
    GLRendererDisplayer::GLRendererDisplayer() : GLRenderer("displayer.frag") {
        setToDisplay();
    }

    void GLRendererDisplayer::update() {
        const GLShader &shader = getShader();
        shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
        shader.upload2i("resolution", getWidth(), getHeight());
    }
}