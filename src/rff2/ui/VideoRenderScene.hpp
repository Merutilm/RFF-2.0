//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "VideoRenderSceneRenderer.hpp"
#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../attr/Attribute.h"
#include "../io/RFFDynamicMapBinary.h"

namespace merutilm::rff2 {
    class VideoRenderScene final : vkh::EngineHandler {

        vkh::WindowContextRef wc;
        RFFBinary *normal = nullptr;
        RFFBinary *zoomed = nullptr;
        bool isStatic = false;
        float currentFrame = 0;
        uint32_t frameIndex = 0;
        const VkExtent2D videoExtent;
        const Attribute targetAttribute;
        std::unique_ptr<VideoRenderSceneRenderer> renderer = nullptr;
        cv::Mat currentImage;

    public:
        explicit VideoRenderScene(vkh::EngineRef engine, vkh::WindowContextRef wc, const VkExtent2D &videoExtent, const Attribute &targetAttribute);

        ~VideoRenderScene() override;

        VideoRenderScene(const VideoRenderScene &) = delete;

        VideoRenderScene &operator=(const VideoRenderScene &) = delete;

        VideoRenderScene(VideoRenderScene &&) = delete;

        VideoRenderScene &operator=(VideoRenderScene &&) = delete;


        void applyCurrentFrame() const;

        void applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed,
                                    float currentSec) const;

        void applyShader() const;

        void setCurrentFrame(float currentFrame);

        void setStatic(bool isStatic);

        void setMap(RFFBinary *normal, RFFBinary *zoomed);

        void applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed, float currentSec) const;

        void initRenderContext() const;

        void initRenderer();

        void applySize() const;

        void refreshSharedImgContext() const;

        void renderOnce() const;

        [[nodiscard]] float calculateZoom(float defaultZoomIncrement) const;

        [[nodiscard]] cv::Mat generateImage() const;

        void init() override;

        void destroy() override;
    };
}
