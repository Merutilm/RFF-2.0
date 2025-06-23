//
// Created by Merutilm on 2025-06-12.
//

#include "VideoRenderScene.h"

#include "../settings/Settings.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/hal/interface.h"


namespace merutilm::rff {
    VideoRenderScene::VideoRenderScene() : WGLScene() {
    }


    void VideoRenderScene::applyCurrentFrame() const {
        rendererIteration2Map->setCurrentFrame(currentFrame);
        rendererStatic2Image->setCurrentFrame(currentFrame);
    }

    void VideoRenderScene::applyCurrentDynamicMap(const RFFDynamicMap &normal, const RFFDynamicMap &zoomed) const {

        auto &normalI = normal.getMatrix();

        if (currentFrame < 1) {
            const double maxIteration = static_cast<double>(normal.getMaxIteration());
            const std::vector<double> zoomedDefault(normalI.getLength());
            rendererIteration2Map->setAllIterations(normalI.getCanvas(), zoomedDefault);
            rendererIteration2Map->setMaxIteration(maxIteration);
            rendererIteration->setMaxIteration(maxIteration);
        } else {
            auto &zoomedI = zoomed.getMatrix();
            const double maxIteration = static_cast<double>(std::min(zoomed.getMaxIteration(), normal.getMaxIteration()));
            rendererIteration2Map->setAllIterations(normalI.getCanvas(), zoomedI.getCanvas());
            rendererIteration2Map->setMaxIteration(maxIteration);
            rendererIteration->setMaxIteration(maxIteration);
        }
    }

    void VideoRenderScene::applyColor(const Settings &settings) const {
        rendererIteration2Map->setDataSettings(settings.videoSettings.dataSettings);
        rendererStatic2Image->setDataSettings(settings.videoSettings.dataSettings);
        rendererIteration->setPaletteSettings(settings.shaderSettings.paletteSettings);
        rendererStripe->setStripeSettings(settings.shaderSettings.stripeSettings);
        rendererSlope->setSlopeSettings(settings.shaderSettings.slopeSettings);
        rendererColor->setColorSettings(settings.shaderSettings.colorSettings);
        rendererFog->setFogSettings(settings.shaderSettings.fogSettings);
        rendererBloom->setBloomSettings(settings.shaderSettings.bloomSettings);
        rendererAntialiasing->setUse(settings.renderSettings.antialiasing);
    }

    void VideoRenderScene::configure(const HWND wnd, const HDC hdc, const HGLRC context) {
        WGLScene::configure(wnd, hdc, context);
        makeContextCurrent();

        rendererStatic = std::make_unique<GLMultipassRenderer>();
        rendererStatic2Image = std::make_unique<GLRendererStatic2Image>();

        rendererStatic->add(*rendererStatic2Image);

        renderer = std::make_unique<GLMultipassRenderer>();
        rendererIteration2Map = std::make_unique<GLRendererIteration2Map>();
        rendererIteration = std::make_unique<GLRendererIteration>();
        rendererStripe = std::make_unique<GLRendererStripe>();
        rendererSlope = std::make_unique<GLRendererSlope>();
        rendererColor = std::make_unique<GLRendererColor>();
        rendererFog = std::make_unique<GLRendererFog>();
        rendererBloom = std::make_unique<GLRendererBloom>();
        rendererAntialiasing = std::make_unique<GLRendererAntialiasing>();

        renderer->add(*rendererIteration2Map);
        renderer->add(*rendererIteration);
        renderer->add(*rendererStripe);
        renderer->add(*rendererSlope);
        renderer->add(*rendererColor);
        renderer->add(*rendererFog);
        renderer->add(*rendererBloom);
        renderer->add(*rendererAntialiasing);
    }

    float VideoRenderScene::calculateZoom(const double defaultZoomIncrement) const {
        if (currentFrame < 1) {
            const float r = 1 - currentFrame;

            if (!normal->hasData()) {
                return 0;
            }

            const float z1 = normal->getLogZoom();
            return std::lerp(z1, z1 + std::log10(defaultZoomIncrement), r);
        }
        const int f1 = static_cast<int>(currentFrame); // it is smaller
        const int f2 = f1 + 1;
        //frame size : f1 = 1x, f2 = 2x
        const float r = f2 - currentFrame;

        if (!zoomed->hasData() || !normal->hasData()) {
            return 0;
        }

        const float z1 = zoomed->getLogZoom();
        const float z2 = normal->getLogZoom();
        return std::lerp(z2, z1, r);
    }

    GLMultipassRenderer &VideoRenderScene::getStaticRenderer() const {
        return *rendererStatic;
    }

    GLMultipassRenderer &VideoRenderScene::getDynamicRenderer() const {
        return *renderer;
    }

    void VideoRenderScene::setCurrentFrame(const float currentFrame) {
        this->currentFrame = currentFrame;
    }

    void VideoRenderScene::setStatic(const bool isStatic){
        this->isStatic = isStatic;
    }

    void VideoRenderScene::setMap(RFFMap *normal, RFFMap *zoomed) {
        this->normal = normal;
        this->zoomed = zoomed;
    }

    void VideoRenderScene::applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed) const {
        rendererStatic2Image->setImageBuffer(normal, zoomed);
    }

    void VideoRenderScene::reloadSize(const uint16_t cw, const uint16_t ch, const uint16_t iw, const uint16_t ih) {
        rendererIteration->reloadIterationBuffer(iw, ih);
        rendererIteration2Map->reloadIterationBuffer(iw, ih);
        renderer->reloadSize(cw, ch, iw, ih);
        rendererStatic->reloadSize(cw, ch, iw, ih);
        reloadImageBuffer(iw, ih);
    }


    void VideoRenderScene::reloadImageBuffer(const uint16_t w, const uint16_t h) {
        pixels = std::vector<char>(static_cast<uint32_t>(w) * h * 3);
        currentImage = cv::Mat(h, w, CV_8UC3, pixels.data());
    }


    void VideoRenderScene::renderGL() {
        glClear(GL_COLOR_BUFFER_BIT);

        GLMultipassRenderer *targetRenderer;

        if (isStatic) {
            targetRenderer = rendererStatic.get();
        }else {
            targetRenderer = renderer.get();
        }

        targetRenderer->render();
        targetRenderer->display();
        swapBuffers();

        const GLuint fbo = targetRenderer->getRenderedFBO();
        const GLuint fboID = targetRenderer->getRenderedFBOTexID();
        const int w = targetRenderer->getWidth();
        const int h = targetRenderer->getHeight();
        GLRenderer::bindFBO(fbo, fboID);
        glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());
        cv::flip(currentImage, currentImage, 0);
        GLRenderer::unbindFBO(fbo);
    }


    const cv::Mat &VideoRenderScene::getCurrentImage() const {
        return currentImage;
    }
}
