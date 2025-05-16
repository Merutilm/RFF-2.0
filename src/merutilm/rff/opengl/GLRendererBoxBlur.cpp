//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererBoxBlur.h"

GLRendererBoxBlur::GLRendererBoxBlur(const std::string_view name) : GLRenderer(name){

}


void GLRendererBoxBlur::setAdditionalParams(const std::function<void(GLShaderLoader&)> &additionalParams) {
    this->additionalParams = additionalParams;
}


void GLRendererBoxBlur::update() {
    GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.upload2i("resolution", getWidth(), getHeight());
    additionalParams(shader);
}

