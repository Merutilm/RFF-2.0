//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <queue>

#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../io/RFFDynamicMapBinary.h"
#include "../settings/Settings.h"
#include "VideoBufferCache.hpp"
#include "VideoRenderSceneRenderer.hpp"

namespace merutilm::rff2 {
    class VideoRenderScene final : vkh::EngineHandler {

        vkh::WindowContextRef wc;
        RFFBinary *normal = nullptr;
        RFFBinary *zoomed = nullptr;
        const VkExtent2D videoExtent;
        const Settings targetSettings;
        std::unique_ptr<VideoRenderSceneRenderer> renderer = nullptr;

    public:
        explicit VideoRenderScene(vkh::EngineRef engine, vkh::WindowContextRef wc, const VkExtent2D &videoExtent, const Settings &targetSettings);

        ~VideoRenderScene() override;

        VideoRenderScene(const VideoRenderScene &) = delete;

        VideoRenderScene &operator=(const VideoRenderScene &) = delete;

        VideoRenderScene(VideoRenderScene &&) = delete;

        VideoRenderScene &operator=(VideoRenderScene &&) = delete;

        void applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed, float currentFrame) const;

        void setMaxIterationDynamic(double maxIteration) const;

        void applyShader() const;

        void setTime(float currentSec) const;

        void setCurrentFrame(float currentFrame) const;

        void setStatic(bool isStatic) const;

        void setMap(RFFBinary *normal, RFFBinary *zoomed);

        void applyCurrentStaticImage(const cv::Mat &normal, const cv::Mat &zoomed) const;

        void initRenderContext() const;

        void initRenderer();

        void applySize() const;

        VkExtent2D getBlurredImageExtent() const;

        void refreshSharedImgContext() const;

        void renderOnce() const;

        [[nodiscard]] const VideoRenderSceneRenderer &getRenderer() const {
            return *renderer;
        }

        [[nodiscard]] float calculateZoom(float defaultZoomIncrement, float currentFrame) const;

        VideoBufferCache createImage() const;



        void init() override;

        void destroy() override;
    };
}
