
#pragma once

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

    virtual ~GLRenderer() = default;
    virtual void update() = 0;

    void beforeUpdate() const;

    void afterUpdate() const;

public:
    int getWidth() const;

    int getHeight() const;

    GLuint getPrevFBOTextureID() const;

    void setAsLastFBO();

    void resetFBOTexture(int panelWidth, int panelHeight);

    int getFBOTextureID() const;

    void bindFBO() const;

    void unbindFBO() const;

    void setPreviousFBOTextureID(GLuint previousFBOTextureID);

    GLShaderLoader &getShaderLoader();

    void reloadSize(int width, int height);

    void render();

    virtual void destroy();
};
