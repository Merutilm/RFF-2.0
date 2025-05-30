#pragma once

#include <mutex>

#include "GLShaderLoader.h"


class GLRenderer {
    GLShaderLoader shader;
    int w = 0;
    int h = 0;
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
    int getWidth() const;

    int getHeight() const;

    GLuint getPrevFBOTextureID() const;

    void setToDisplay();

    void resetFBOTexture(int width, int height);

    GLuint getFBO() const;

    GLuint getFBOTextureID() const;

    static void bindFBO(GLuint fbo, GLuint fboTextureID);

    static void unbindFBO(GLuint fbo);

    virtual void setPreviousFBOTextureID(GLuint previousFBOTextureID);

    GLShaderLoader &getShaderLoader();

    virtual void reloadSize(int width, int height);

    void render();
};
