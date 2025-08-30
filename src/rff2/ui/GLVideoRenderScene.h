//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include "../io/RFFDynamicMapBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "../attr/Attribute.h"
#include "opencv2/core/mat.hpp"


namespace merutilm::rff2 {
    class GLVideoRenderScene final {
        RFFBinary *normal = nullptr;
        RFFBinary *zoomed = nullptr;
        bool isStatic = false;
        std::vector<char> pixels = std::vector<char>();
        cv::Mat currentImage;
        float currentFrame = 0;

        // std::unique_ptr<GLMultipassRenderer> rendererStatic;
        // std::unique_ptr<GLRendererStatic2Image> rendererStatic2Image;
        //
        // std::unique_ptr<GLMultipassRenderer> renderer;
        // std::unique_ptr<GLRendererShdIteration2Map> rendererShdIteration2Map;
        // std::unique_ptr<GLRendererColIteration2Map> rendererColIteration2Map;
        // std::unique_ptr<GLRendererStripe> rendererStripe;
        // std::unique_ptr<GLRendererSlope> rendererSlope;
        // std::unique_ptr<GLRendererColor> rendererColor;
        // std::unique_ptr<GLRendererFog> rendererFog;
        // std::unique_ptr<GLRendererBloom> rendererBloom;
        // std::unique_ptr<GLRendererAntialiasing> rendererAntialiasing;

    public:
        GLVideoRenderScene();

        void applyCurrentFrame() const;

        void applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed) const;

        void applyColor(const Attribute &settings) const;

        // void configure(HWND wnd, HDC hdc, HGLRC context);

        float calculateZoom(double defaultZoomIncrement) const;

        // GLMultipassRenderer &getStaticRenderer() const;
        //
        // GLMultipassRenderer &getDynamicRenderer() const;

        void setCurrentFrame(float currentFrame);

        void setStatic(bool isStatic);

        void setMap(RFFBinary *normal, RFFBinary *zoomed);

        void applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed) const;

        void reloadSize(uint16_t cw, uint16_t ch, uint16_t iw, uint16_t ih);

        void reloadImageBuffer(uint16_t w, uint16_t h);

        // void renderGL() override;

        const cv::Mat &getCurrentImage() const;
    };
}
