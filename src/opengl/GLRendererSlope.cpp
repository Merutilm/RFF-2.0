//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererSlope.h"


namespace merutilm::rff2 {
    GLRendererSlope::GLRendererSlope() : GLRenderer("slope.frag"){

    }

    void GLRendererSlope::setIterationTextureID(const GLuint textureID) {
        iterationTextureID = textureID;
    }


    void GLRendererSlope::setSlopeSettings(const SlopeSettings &slopeSettings) {
        this->slopeSettings = &slopeSettings;
    }

    void GLRendererSlope::setClarityMultiplier(const float clarityMultiplier) {
        this->clarityMultiplier = clarityMultiplier;
    }

    void GLRendererSlope::update() {
        const GLShader &shader = getShader();
        shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
        shader.uploadTexture2D("iterations", GL_TEXTURE1, iterationTextureID);
        shader.uploadFloat("depth", slopeSettings->depth);
        shader.uploadFloat("clarityMultiplier", clarityMultiplier);
        shader.uploadFloat("reflectionRatio", slopeSettings->reflectionRatio);
        shader.uploadFloat("opacity", slopeSettings->opacity);
        shader.uploadFloat("zenith", slopeSettings->zenith);
        shader.uploadFloat("azimuth", slopeSettings->azimuth);
    }
}







