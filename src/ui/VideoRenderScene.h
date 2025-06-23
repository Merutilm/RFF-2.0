//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include "WGLScene.h"
#include "../io/RFFDynamicMap.h"
#include "../io/RFFStaticMap.h"
#include "../opengl/GLMultipassRenderer.h"
#include "../opengl/GLRendererAntialiasing.h"
#include "../opengl/GLRendererBloom.h"
#include "../opengl/GLRendererColor.h"
#include "../opengl/GLRendererFog.h"
#include "../opengl/GLRendererIteration.h"
#include "../opengl/GLRendererIteration2Map.h"
#include "../opengl/GLRendererSlope.h"
#include "../opengl/GLRendererStatic2Image.h"
#include "../opengl/GLRendererStripe.h"
#include "../settings/Settings.h"
#include "opencv2/core/mat.hpp"


namespace merutilm::rff {
    class VideoRenderScene final : public WGLScene {
        RFFMap *normal = nullptr;
        RFFMap *zoomed = nullptr;
        bool isStatic = false;
        std::vector<char> pixels = std::vector<char>();
        cv::Mat currentImage;
        float currentFrame = 0;

        std::unique_ptr<GLMultipassRenderer> rendererStatic;
        std::unique_ptr<GLRendererStatic2Image> rendererStatic2Image;

        std::unique_ptr<GLMultipassRenderer> renderer;
        std::unique_ptr<GLRendererIteration2Map> rendererIteration2Map;
        std::unique_ptr<GLRendererIteration> rendererIteration;
        std::unique_ptr<GLRendererStripe> rendererStripe;
        std::unique_ptr<GLRendererSlope> rendererSlope;
        std::unique_ptr<GLRendererColor> rendererColor;
        std::unique_ptr<GLRendererFog> rendererFog;
        std::unique_ptr<GLRendererBloom> rendererBloom;
        std::unique_ptr<GLRendererAntialiasing> rendererAntialiasing;

    public:
        VideoRenderScene();

        void applyCurrentFrame() const;

        void applyCurrentDynamicMap(const RFFDynamicMap &normal, const RFFDynamicMap &zoomed) const;

        void applyColor(const Settings &settings) const;

        void configure(HWND wnd, HDC hdc, HGLRC context) override;

        float calculateZoom(double defaultZoomIncrement) const;

        GLMultipassRenderer &getStaticRenderer() const;

        GLMultipassRenderer &getDynamicRenderer() const;

        void setCurrentFrame(float currentFrame);

        void setStatic(bool isStatic);

        void setMap(RFFMap *normal, RFFMap *zoomed);

        void applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed) const;

        void reloadSize(uint16_t cw, uint16_t ch, uint16_t iw, uint16_t ih);

        void reloadImageBuffer(uint16_t w, uint16_t h);

        void renderGL() override;

        const cv::Mat &getCurrentImage() const;
    };
}
