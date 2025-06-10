#pragma once

#include <mutex>

#include "GLShaderLoader.h"


class GLRenderer {
    GLShaderLoader shader;
    uint16_t w = 0;
    uint16_t h = 0;
    GLuint previousFBOTextureID = 0;
    GLuint fbo;
    GLuint fboTextureID;
    std::mutex mutex;

protected:
    explicit GLRenderer(std::string_view name);

    virtual ~GLRenderer();

    virtual void update() = 0;

    virtual void beforeUpdate();

    virtual void afterUpdate();

public:
    uint16_t getWidth() const;

    uint16_t getHeight() const;

    GLuint getPrevFBOTextureID() const;

    void setToDisplay();

    void resetFBOTexture(uint16_t width, uint16_t height);

    GLuint getFBO() const;

    GLuint getFBOTextureID() const;

    static void bindFBO(GLuint fbo, GLuint fboTextureID);

    static void unbindFBO(GLuint fbo);

    virtual void setPreviousFBOTextureID(GLuint previousFBOTextureID);

    GLShaderLoader &getShaderLoader();

    virtual void reloadSize(uint16_t width, uint16_t height);

    void render();
};
