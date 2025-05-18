//
// Created by Merutilm on 2025-05-14.
//

#include "GLMultipassRenderer.h"

#include "GLIterationTextureProvider.h"
#include "GLIterationTextureRenderer.h"
#include "GLTimeRenderer.h"


void GLMultipassRenderer::add(GLRenderer *renderer) {
    renderers.emplace_back(renderer);
}


void GLMultipassRenderer::reloadSize(const int w, const int h) const {
    for (GLRenderer *renderer: renderers) {
        renderer->reloadSize(w, h);
    }
}

void GLMultipassRenderer::setTime(const float timeSec) {
    this->timeSec = timeSec;
}

void GLMultipassRenderer::render() const {
    int iterationTextureID = 0;
    float resolutionMultiplier = 1;

    for (int i = 0; i < renderers.size(); ++i) {
        GLRenderer *renderer = renderers[i];

        if (const auto k = dynamic_cast<GLIterationTextureProvider *>(renderer);
            k != nullptr) {
            iterationTextureID = k->getIterationTextureID();
            resolutionMultiplier = k->getClarityMultiplier();
        }
        if (const auto r = dynamic_cast<GLIterationTextureRenderer *>(renderer)) {
            r->setIterationTextureID(iterationTextureID);
            r->setClarityMultiplier(resolutionMultiplier);
        }
        if (const auto t = dynamic_cast<GLTimeRenderer *>(renderer)) {
            t->setTime(timeSec);
        }
        if (i >= 1) {
            renderer->setPreviousFBOTextureID(renderers[i - 1]->getFBOTextureID());
        }
        if (i == renderers.size() - 1) {
            renderer->setAsLastFBO();
        }
        renderer->render();
    }
}
