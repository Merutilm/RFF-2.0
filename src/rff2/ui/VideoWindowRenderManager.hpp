//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <queue>

#include "../io/RFFDynamicMapBinary.h"
#include "../settings/Settings.h"
#include "VideoBufferCache.hpp"
#include "VideoWindowRenderer.hpp"
#include "vulkan_helper/engine/SharedResource.hpp"
#include "vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::rff2 {
    class VideoWindowRenderManager final : vkh::EngineHandler {

        vkh::WindowContext &wc;
        vkh::SharedResource &sr;
        RFFBinary *normal = nullptr;
        RFFBinary *zoomed = nullptr;
        const VkExtent2D videoExtent;
        const Settings targetSettings;
        std::unique_ptr<VideoWindowRenderer> renderer = nullptr;

    public:
        explicit VideoWindowRenderManager(vkh::Engine &engine, vkh::WindowContext &wc, vkh::SharedResource &sr, const VkExtent2D &videoExtent, const Settings &targetSettings);

        ~VideoWindowRenderManager() override;

        VideoWindowRenderManager(const VideoWindowRenderManager &) = delete;

        VideoWindowRenderManager &operator=(const VideoWindowRenderManager &) = delete;

        VideoWindowRenderManager(VideoWindowRenderManager &&) = delete;

        VideoWindowRenderManager &operator=(VideoWindowRenderManager &&) = delete;

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

        [[nodiscard]] const VideoWindowRenderer &getRenderer() const {
            return *renderer;
        }

        [[nodiscard]] const vkh::WindowContext &getWindowContext() const {
            return wc;
        }

        [[nodiscard]] float calculateZoom(float defaultZoomIncrement, float currentFrame) const;

        VideoBufferCache createImage() const;



        void init() override;

        void cleanup() override;
    };
}
