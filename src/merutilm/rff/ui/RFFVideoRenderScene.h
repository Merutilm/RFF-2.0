//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include "RFFScene.h"
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

class RFFVideoRenderScene final : public RFFScene {

    RFFMap normal = RFFMap::DEFAULT_MAP;
    RFFMap zoomed = RFFMap::DEFAULT_MAP;
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
    RFFVideoRenderScene();

    void applyCurrentMap() const;

    void applyColor(const Settings &settings) const;

    void configure(HWND wnd, HDC hdc, HGLRC context) override;

    float calculateZoom(double defaultZoomIncrement) const;

    GLMultipassRenderer &getRenderer() const;

    void setMap(float currentFrame, RFFMap &&normal, RFFMap &&zoomed);

    void renderGL() override;

    const cv::Mat &getCurrentImage() const;

};
