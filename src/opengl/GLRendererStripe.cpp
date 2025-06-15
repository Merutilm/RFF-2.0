//
// Created by Merutilm on 2025-05-14.
//

#include "GLRendererStripe.h"


merutilm::rff::GLRendererStripe::GLRendererStripe() : GLRenderer("stripe.frag"){

}

void merutilm::rff::GLRendererStripe::setIterationTextureID(const GLuint textureID) {
    iterationTextureID = textureID;
}

void merutilm::rff::GLRendererStripe::setStripeSettings(const StripeSettings &stripeSettings) {
    this->stripeSettings = &stripeSettings;
}


void merutilm::rff::GLRendererStripe::setTime(const float time) {
    this->time = time;
}

void merutilm::rff::GLRendererStripe::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.uploadTexture2D("inputTex", GL_TEXTURE0, getPrevFBOTextureID());
    shader.uploadTexture2D("iterations", GL_TEXTURE1, iterationTextureID);

    shader.uploadInt("type", static_cast<int>(stripeSettings->stripeType));
    shader.uploadFloat("firstInterval", stripeSettings->firstInterval);
    shader.uploadFloat("secondInterval", stripeSettings->secondInterval);
    shader.uploadFloat("opacity", stripeSettings->opacity);
    shader.uploadFloat("offset", stripeSettings->offset + stripeSettings->animationSpeed * time);
}








