//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <nppdefs.h>
#include <queue>

#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../io/RFFDynamicMapBinary.h"
#include "../settings/Settings.h"
#include "VideoBufferCache.hpp"
#include "VideoRenderSceneRenderer.hpp"

extern "C"{
    #include "libavutil/frame.h"
}

namespace merutilm::rff2 {
    class VideoRenderScene final : vkh::EngineHandler {

        vkh::WindowContextRef wc;
        RFFBinary *normal = nullptr;
        RFFBinary *zoomed = nullptr;
        const VkExtent2D videoExtent;
        const Settings targetSettings;
        std::unique_ptr<VideoRenderSceneRenderer> renderer = nullptr;

        std::mutex bufferCachedMutex;
        std::queue<std::unique_ptr<VideoBufferCache>> queuedVbc = {};
        std::condition_variable bufferCachedCondition;

    public:
        explicit VideoRenderScene(vkh::EngineRef engine, vkh::WindowContextRef wc, const VkExtent2D &videoExtent,
                                  const Settings &targetSettings);

        ~VideoRenderScene() override;

        VideoRenderScene(const VideoRenderScene &) = delete;

        VideoRenderScene &operator=(const VideoRenderScene &) = delete;

        VideoRenderScene(VideoRenderScene &&) = delete;

        VideoRenderScene &operator=(VideoRenderScene &&) = delete;

        void applyCurrentDynamicMap(const RFFDynamicMapBinary &normal, const RFFDynamicMapBinary &zoomed,
                                    float currentFrame) const;

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

        [[nodiscard]] const VideoRenderSceneRenderer &getRenderer() const { return *renderer; }

        [[nodiscard]] float calculateZoom(float defaultZoomIncrement, float currentFrame) const;

        void queueImage();

        const vkh::BufferContext &getCurrentBufferWithSync(uint32_t *frameIndex) const;

        void fillCurrentImgToFrame(const AVFrame *frame, const NppStreamContext &streamCtx) const;

        HANDLE getBufferExtHandle(const vkh::BufferContext &buf) const;

        [[nodiscard]] std::mutex &getBufferCachedMutex() { return bufferCachedMutex; }

        [[nodiscard]] std::condition_variable &getBufferCachedCondition() { return bufferCachedCondition; }

        [[nodiscard]] std::queue<std::unique_ptr<VideoBufferCache>> &getQueuedBuffers() { return queuedVbc; }


        void init() override;

        void destroy() override;
    };
} // namespace merutilm::rff2
