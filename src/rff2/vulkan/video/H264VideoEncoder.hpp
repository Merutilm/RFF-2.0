//
// Created by Merutilm on 10/7/26.
// Modified by Claude Sonnet 5.5 on 10/8/26.
// Re-Implemented by Merutilm on 10/8/26.
//

#pragma once

#include "../../settings/VideoSettings.h"
#include "vulkan_helper/handle/EngineHandler.hpp"

#include <optional>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/hwcontext.h>
#include <libavutil/hwcontext_vulkan.h>
#include <libavutil/pixfmt.h>
}


namespace merutilm::rff2 {
    class H264VideoEncoder : public vkh::EngineHandler {

        vkh::CommandPool &commandPool;
        vkh::CommandBuffer cb;
        vkh::Fence fence;

        bool initialized = false;

        const VkExtent2D extent;
        const VideoSettings videoSettings;
        std::string filename;

        AVBufferRef *hwDeviceCtx = nullptr;
        AVBufferRef *hwFramesCtx = nullptr;

        AVCodecContext *codecCtx = nullptr;
        AVFormatContext *formatCtx = nullptr;
        AVStream *stream = nullptr;

        int64_t frameIndex = 0;

        VkPhysicalDeviceVulkan13Features feats13{};
        VkPhysicalDeviceVulkan12Features feats12{};
        VkPhysicalDeviceFeatures2 feats2{};

    public:
        explicit H264VideoEncoder(vkh::Engine &engine, vkh::CommandPool &commandPool, const VkExtent2D extent, VideoSettings videoSettings,
                                  std::string filename) :
            EngineHandler(engine), commandPool(commandPool), cb(engine.getCore(), commandPool), fence(engine.getCore()), extent(extent), videoSettings(std::move(videoSettings)),
            filename(std::move(filename)) {
            H264VideoEncoder::init();
        }

        ~H264VideoEncoder() override { H264VideoEncoder::cleanup(); }

        H264VideoEncoder(const H264VideoEncoder &) = delete;
        H264VideoEncoder &operator=(const H264VideoEncoder &) = delete;
        H264VideoEncoder(H264VideoEncoder &&) = delete;
        H264VideoEncoder &operator=(H264VideoEncoder &&) = delete;

        [[nodiscard]] bool isInitialized() const { return initialized; }

        friend H264VideoEncoder &operator<<(H264VideoEncoder &in, const vkh::ImageContext &ctx) {
            in.encode(ctx);
            return in;
        }

        void encode(const vkh::ImageContext &ctx);

    protected:
        bool convertToEncodeFrame(const vkh::ImageContext &src, AVVkFrame &dst) const;

        static std::optional<uint32_t> findEncodeQueueFamily(VkPhysicalDevice phys);

        static void lockQueue(AVHWDeviceContext *ctx, uint32_t, uint32_t);

        static void unlockQueue(AVHWDeviceContext *ctx, uint32_t, uint32_t);

        bool initDevice();

        bool initFramesCtx();

        bool initFormat();

        bool initEncoder();

        bool initStream();


        void drainPackets() const;

        void flushEncoder() const;

        static std::string ffmpegErrorString(int error);

        static void ffmpegError(const char *operation, int error);


        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::rff2
