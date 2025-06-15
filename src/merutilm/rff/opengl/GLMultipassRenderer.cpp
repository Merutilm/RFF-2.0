//
// Created by Merutilm on 2025-05-14.
//

#include "GLMultipassRenderer.h"

#include "GLIterationTextureProvider.h"
#include "GLIterationTextureRenderer.h"
#include "GLTimeRenderer.h"


merutilm::rff::GLMultipassRenderer::GLMultipassRenderer() : displayer(std::make_unique<GLRendererDisplayer>()){

}


void merutilm::rff::GLMultipassRenderer::add(GLRenderer &renderer) {
    renderers.emplace_back(&renderer);
}


void merutilm::rff::GLMultipassRenderer::reloadSize(const int cw, const int ch, const int iw, const int ih) {
    for (GLRenderer *renderer: renderers) {
        renderer->reloadSize(iw, ih);
    }
    displayer->reloadSize(cw, ch);
    width = iw;
    height = ih;
}

void merutilm::rff::GLMultipassRenderer::setTime(const float timeSec) {
    this->timeSec = timeSec;
}



void merutilm::rff::GLMultipassRenderer::render() const {
    int iterationTextureID = 0;
    for (int i = 0; i < renderers.size(); ++i) {
        GLRenderer *rendererPtr = renderers[i];

        if (rendererPtr == nullptr) {
            continue;
        }

        GLRenderer &renderer = *rendererPtr;
        if (const auto k = dynamic_cast<GLIterationTextureProvider *>(&renderer);
            k != nullptr && iterationTextureID == 0) {
            iterationTextureID = k->getIterationTextureID();
        }
        if (const auto r = dynamic_cast<GLIterationTextureRenderer *>(&renderer)) {
            r->setIterationTextureID(iterationTextureID);
        }
        if (const auto t = dynamic_cast<GLTimeRenderer *>(&renderer)) {
            t->setTime(timeSec);
        }
        if (i >= 1) {
            renderer.setPreviousFBOTextureID(renderers[i - 1]->getFBOTextureID());
        }
        renderer.render();
    }
}

void merutilm::rff::GLMultipassRenderer::display() {
    renderedFBO = renderers.back()->getFBO();
    renderedFBOTexID = renderers.back()->getFBOTextureID();
    displayer->setPreviousFBOTextureID(renderedFBOTexID);
    displayer->render();
}


int merutilm::rff::GLMultipassRenderer::getWidth() const {
    return width;
}

int merutilm::rff::GLMultipassRenderer::getHeight() const {
    return height;
}

GLuint merutilm::rff::GLMultipassRenderer::getRenderedFBO() const {
    return renderedFBO;
}

GLuint merutilm::rff::GLMultipassRenderer::getRenderedFBOTexID() const {
    return renderedFBOTexID;
}
