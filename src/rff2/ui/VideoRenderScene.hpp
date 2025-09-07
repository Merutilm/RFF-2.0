//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "VideoRenderSceneShaderPrograms.hpp"
#include "../attr/Attribute.h"
#include "../io/RFFDynamicMapBinary.h"

namespace merutilm::rff2 {
    class VideoRenderScene final : vkh::WindowContextHandler {
        RFFBinary *normal = nullptr;
        RFFBinary *zoomed = nullptr;
        bool isStatic = false;
        std::vector<char> pixels = std::vector<char>();
        cv::Mat currentImage;
        float currentFrame = 0;
        uint32_t frameIndex = 0;

        std::unique_ptr<VideoRenderSceneShaderPrograms> shaderPrograms = nullptr;

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
        explicit VideoRenderScene(vkh::WindowContextRef wc);

        ~VideoRenderScene() override;

        VideoRenderScene(const VideoRenderScene &) = delete;

        VideoRenderScene &operator=(const VideoRenderScene &) = delete;

        VideoRenderScene(VideoRenderScene &&) = delete;

        VideoRenderScene &operator=(VideoRenderScene &&) = delete;


        void applyCurrentFrame() const;

        void applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed,
                                    float currentSec) const;

        void applyShader(const Attribute &attr) const;

        void draw(uint32_t swapchainImageIndex) const;

        void setCurrentFrame(float currentFrame);

        void setStatic(bool isStatic);

        void setMap(RFFBinary *normal, RFFBinary *zoomed);

        void applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed, float currentSec) const;

        void reloadSize(uint16_t cw, uint16_t ch, uint16_t iw, uint16_t ih);

        void reloadImageBuffer(uint16_t w, uint16_t h);

        void renderOnce();

        [[nodiscard]] float calculateZoom(double defaultZoomIncrement) const;

        [[nodiscard]] const cv::Mat &getCurrentImage() const;

        void init() override;

        void destroy() override;
    };
}
