//
// Created by user on 2025-05-01.
//

#include "GLRenderer.h"

#include <iostream>
#include <mutex>


GLRenderer::GLRenderer(const std::string_view name) : shader(
    GLShaderLoader(RFF::GLConfig::VERTEX_PATH_DEFAULT, name)) {
    //-----------------------------------------------
    //Generate FBO
    //-----------------------------------------------
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &fboTextureID);
}


GLRenderer::~GLRenderer() {
    if (glIsFramebuffer(fbo)) {
        glDeleteFramebuffers(1, &fbo);
    }
    if (glIsTexture(fboTextureID)) {
        glDeleteTextures(1, &fboTextureID);
    }
}


void GLRenderer::reloadSize(const uint16_t width, const uint16_t height) {
    this->w = width;
    this->h = height;
    resetFBOTexture(w, h);
}

void GLRenderer::beforeUpdate() {
    bindFBO(fbo, fboTextureID);
    glViewport(0, 0, w, h);
}

void GLRenderer::afterUpdate() {
    unbindFBO(fbo);
    int error = glGetError();
    while (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << " at " << typeid(this).name() << "\n" << std::flush;
        error = glGetError();
    }
}

uint16_t GLRenderer::getWidth() const {
    return w;
}

uint16_t GLRenderer::getHeight() const {
    return h;
}

GLuint GLRenderer::getPrevFBOTextureID() const {
    return previousFBOTextureID;
}


void GLRenderer::setToDisplay() {
    if (fbo == 0) {
        return;
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &fboTextureID);

    fbo = 0;
    fboTextureID = 0;
}


void GLRenderer::resetFBOTexture(const uint16_t width, const uint16_t height) {
    if (fbo == 0) {
        return;
    }
    fboTextureID = GLShaderLoader::recreateTexture2D(fboTextureID, width, height,
                                                     RFF::TextureFormats::FLOAT4);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint GLRenderer::getFBO() const {
    return fbo;
}

GLuint GLRenderer::getFBOTextureID() const {
    return fboTextureID;
}


void GLRenderer::bindFBO(const GLuint fbo, const GLuint fboTextureID) {
    if (glIsFramebuffer(fbo)) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureID, 0);
    }
}


void GLRenderer::unbindFBO(const GLuint fbo) {
    if (glIsFramebuffer(fbo)) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GLRenderer::setPreviousFBOTextureID(const GLuint previousFBOTextureID) {
    this->previousFBOTextureID = previousFBOTextureID;
}


GLShaderLoader &GLRenderer::getShaderLoader() {
    return shader;
}

void GLRenderer::render() {
    std::scoped_lock lock(mutex);
    beforeUpdate();
    shader.use();
    update();
    shader.draw();
    GLShaderLoader::detach();
    afterUpdate();
}
