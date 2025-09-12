//
// Created by Merutilm on 2025-09-12.
//

#pragma once
#include "../../vulkan_helper/context/BufferContext.hpp"
#include "../../vulkan_helper/handle/CoreHandler.hpp"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff2 {
    struct VideoBufferCache final : vkh::CoreHandler{
        vkh::BufferContext bufferContext;
        int width;
        int height;
        float zoom;
        cv::Mat image;

        explicit VideoBufferCache(vkh::CoreRef core, vkh::BufferContext &&ctx, const int width,
                                  const int height, const float zoom) : CoreHandler(core), bufferContext(std::move(ctx)), width(width), height(height), zoom(zoom) {
            VideoBufferCache::init();
        }

        ~VideoBufferCache() override {
            VideoBufferCache::destroy();
        }

        VideoBufferCache(const VideoBufferCache &) = delete;

        VideoBufferCache &operator=(const VideoBufferCache &) = delete;

        VideoBufferCache(VideoBufferCache &&) = delete;

        VideoBufferCache &operator=(VideoBufferCache &&) = delete;


        void init() override {
            image = cv::Mat(height, width, CV_8UC3, bufferContext.mappedMemory);
        }

        void destroy() override {
            vkh::BufferContext::destroyContext(core, bufferContext);
        }
    };
}
