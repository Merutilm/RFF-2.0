//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include "Win32GLScene.h"
#include "../io/RFFMap.h"
#include "../opengl/GLMultipassRenderer.h"
#include "../opengl/GLRendererAntialiasing.h"
#include "../opengl/GLRendererBloom.h"
#include "../opengl/GLRendererColor.h"
#include "../opengl/GLRendererFog.h"
#include "../opengl/GLRendererIteration.h"
#include "../opengl/GLRendererIteration2Map.h"
#include "../opengl/GLRendererSlope.h"
#include "../opengl/GLRendererStripe.h"
#include "../settings/Settings.h"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff {
    class VideoRenderScene final : public Win32GLScene {

        RFFMap normal = RFFMap::DEFAULT_MAP;
        RFFMap zoomed = RFFMap::DEFAULT_MAP;
        std::vector<char> pixels = std::vector<char>();
        cv::Mat currentImage;
        float currentFrame = 0;
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

        void applyCurrentMap() const;

        void applyColor(const Settings &settings) const;

        void configure(HWND wnd, HDC hdc, HGLRC context) override;

        float calculateZoom(double defaultZoomIncrement) const;

        GLMultipassRenderer &getRenderer() const;

        void setCurrentFrame(float currentFrame);

        void setMap(RFFMap &&normal, RFFMap &&zoomed);

        void reloadImageBuffer(uint16_t w, uint16_t h);

        void renderGL() override;

        const cv::Mat &getCurrentImage() const;

    };
}