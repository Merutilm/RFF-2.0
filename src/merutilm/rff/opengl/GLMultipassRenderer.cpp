//
// Created by Merutilm on 2025-05-14.
//

#include "GLMultipassRenderer.h"

#include "GLIterationTextureProvider.h"
#include "GLIterationTextureRenderer.h"
#include "GLTimeRenderer.h"


GLMultipassRenderer::GLMultipassRenderer() : displayer(std::make_unique<GLRendererDisplayer>()){

}


void GLMultipassRenderer::add(GLRenderer &renderer) {
    renderers.emplace_back(&renderer);
}


void GLMultipassRenderer::reloadSize(const int cw, const int ch, const int iw, const int ih) const {
    for (GLRenderer *renderer: renderers) {
        renderer->reloadSize(iw, ih);
    }
    displayer->reloadSize(cw, ch);
}

void GLMultipassRenderer::setTime(const float timeSec) {
    this->timeSec = timeSec;
}



void GLMultipassRenderer::render() const {
    int iterationTextureID = 0;
    for (int i = 0; i < renderers.size(); ++i) {
        GLRenderer *rendererPtr = renderers[i];

        if (rendererPtr == nullptr) {
            continue;
        }

        GLRenderer &renderer = *rendererPtr;
        if (const auto k = dynamic_cast<GLIterationTextureProvider *>(&renderer);
            k != nullptr) {
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

void GLMultipassRenderer::display() const {
    displayer->setPreviousFBOTextureID(renderers.back()->getFBOTextureID());
    displayer->render();
}
