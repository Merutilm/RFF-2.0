//
// Created by Merutilm on 10/7/26.
//

#include "H264VideoEncoder.hpp"
#include <array>
#include <mutex>
#include <vector>

#include "vulkan_helper/engine/executor/ScopedNewCommandBufferExecutor.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"

namespace merutilm::rff2 {


    void H264VideoEncoder::encode(const vkh::ImageContext &ctx) {
        if (!initialized) {
            vkh::logger::log_err("use of initialization-failed object");
            return;
        }

        AVFrame *frame = av_frame_alloc();
        if (!frame) {
            vkh::logger::log_err("av_frame_alloc failed");
            return;
        }

        if (const int r = av_hwframe_get_buffer(hwFramesCtx, frame, 0); r < 0) {
            ffmpegError("av_hwframe_get_buffer", r);
            av_frame_free(&frame);
            return;
        }

        auto *vkFrame = reinterpret_cast<AVVkFrame *>(frame->data[0]);
        auto *fc = reinterpret_cast<AVHWFramesContext *>(hwFramesCtx->data);
        auto *vkfc = static_cast<AVVulkanFramesContext *>(fc->hwctx);

        vkfc->lock_frame(fc, vkFrame);
        const bool converted = convertToEncodeFrame(ctx, *vkFrame);
        vkfc->unlock_frame(fc, vkFrame);

        if (!converted) {
            vkh::logger::log_err("failed to convert image into encode frame");
            av_frame_free(&frame);
            return;
        }

        frame->pts = frameIndex++;
        const int ret = avcodec_send_frame(codecCtx, frame);
        av_frame_free(&frame);

        if (ret < 0) {
            ffmpegError("avcodec_send_frame", ret);
        }
        drainPackets();
    }
    bool H264VideoEncoder::convertToEncodeFrame(const vkh::ImageContext &src, AVVkFrame &dst) const {
        if (dst.img[0] == VK_NULL_HANDLE) {
            vkh::logger::log_err("invalid encode frame (null image/semaphore)");
            return false;
        }
        if (dst.img[1] != VK_NULL_HANDLE) {
            vkh::logger::log_err("per-plane images are not supported (expected single multi-planar NV12 image)");
            return false;
        }

        cb.begin();

        vkh::BarrierUtils::cmdImageMemoryBarrier(cb.getCommandBufferHandle(), src.image,
                                                 VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                                 VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                 VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT, 0, 1,
                                                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        vkh::BarrierUtils::cmdImageMemoryBarrier(
                cb.getCommandBufferHandle(), dst.img[0], dst.access[0],
                VK_ACCESS_TRANSFER_WRITE_BIT, dst.layout[0], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT, 0, 1, 0, VK_PIPELINE_STAGE_TRANSFER_BIT);

        //YUV IMAGE COPY
        std::array<VkImageCopy, 2> regions{};
        const std::array aspects = {VK_IMAGE_ASPECT_PLANE_0_BIT, VK_IMAGE_ASPECT_PLANE_1_BIT};
        for (uint32_t i = 0; i < 2; ++i) {
            regions[i].srcSubresource = {.aspectMask = aspects[i], .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1};
            regions[i].dstSubresource = {.aspectMask = aspects[i], .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1};
            regions[i].extent = {
                    .width = (extent.width + i) / (i + 1), .height = (extent.height + i) / (i + 1), .depth = 1};
        }
        vkCmdCopyImage(cb.getCommandBufferHandle(), src.image,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.img[0], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       regions.size(), regions.data());


        cb.end();
        cb.submit(&fence, {}, {});
        fence.waitAndReset();

        // update layout
        dst.layout[0] = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dst.access[0] = VK_ACCESS_TRANSFER_WRITE_BIT;
        return true;
    }
    std::optional<uint32_t> H264VideoEncoder::findEncodeQueueFamily(const VkPhysicalDevice phys) {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties2(phys, &count, nullptr);

        std::vector video(
                count, VkQueueFamilyVideoPropertiesKHR{.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR});
        std::vector props(count, VkQueueFamilyProperties2{.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});

        for (uint32_t i = 0; i < count; ++i) {
            props[i].pNext = &video[i];
        }
        vkGetPhysicalDeviceQueueFamilyProperties2(phys, &count, props.data());

        for (uint32_t i = 0; i < count; ++i) {
            const bool isEncode = props[i].queueFamilyProperties.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
            const bool isH264 = video[i].videoCodecOperations & VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR;
            if (isEncode && isH264) {
                return i;
            }
        }
        return std::nullopt;
    }
    void H264VideoEncoder::lockQueue(AVHWDeviceContext *ctx, uint32_t, uint32_t) {
        static_cast<H264VideoEncoder *>(ctx->user_opaque)->engine.getCore().getLogicalDevice().getQueueMutex().lock();
    }
    void H264VideoEncoder::unlockQueue(AVHWDeviceContext *ctx, uint32_t, uint32_t) {
        static_cast<H264VideoEncoder *>(ctx->user_opaque)->engine.getCore().getLogicalDevice().getQueueMutex().unlock();
    }
    bool H264VideoEncoder::initDevice() {
        const VkPhysicalDevice phys = engine.getCore().getPhysicalDeviceLoader().getPhysicalDeviceHandle();
        const uint32_t gcFamily =
                *engine.getCore().getPhysicalDeviceLoader().getQueueFamilyIndices().graphicsAndComputeFamily;

        const std::optional<uint32_t> encFamily = findEncodeQueueFamily(phys);
        if (!encFamily.has_value()) {
            vkh::logger::log_err("no H.264 encode queue family");
            return false;
        }

        hwDeviceCtx = av_hwdevice_ctx_alloc(AV_HWDEVICE_TYPE_VULKAN);

        if (!hwDeviceCtx) {
            vkh::logger::log_err("av_hwdevice_ctx_alloc(Vulkan) failed");
            return false;
        }

        auto *hwCtx = reinterpret_cast<AVHWDeviceContext *>(hwDeviceCtx->data);
        auto *vkCtx = static_cast<AVVulkanDeviceContext *>(hwCtx->hwctx);

        hwCtx->user_opaque = this;

        vkCtx->get_proc_addr = vkGetInstanceProcAddr;
        vkCtx->alloc = nullptr;
        vkCtx->inst = engine.getCore().getInstance().getInstanceHandle();
        vkCtx->phys_dev = phys;
        vkCtx->act_dev = engine.getCore().getLogicalDevice().getLogicalDeviceHandle();

        feats13 = VkPhysicalDeviceVulkan13Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        feats12 = VkPhysicalDeviceVulkan12Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        feats12.pNext = &feats13;
        feats2 = VkPhysicalDeviceFeatures2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        feats2.pNext = &feats12;
        vkGetPhysicalDeviceFeatures2(phys, &feats2);
        vkCtx->device_features = feats2;

        vkCtx->enabled_inst_extensions = nullptr;
        vkCtx->nb_enabled_inst_extensions = 0;
        vkCtx->enabled_dev_extensions =
                engine.getCore().getInstance().getApplicationCreateInfo().deviceExtensions.data();
        vkCtx->nb_enabled_dev_extensions =
                static_cast<int>(engine.getCore().getInstance().getApplicationCreateInfo().deviceExtensions.size());

        vkCtx->queue_family_index = static_cast<int>(gcFamily);
        vkCtx->nb_graphics_queues = 1;
        vkCtx->queue_family_tx_index = static_cast<int>(gcFamily);
        vkCtx->nb_tx_queues = 1;
        vkCtx->queue_family_comp_index = static_cast<int>(gcFamily);
        vkCtx->nb_comp_queues = 1;
        vkCtx->queue_family_encode_index = static_cast<int>(*encFamily);
        vkCtx->nb_encode_queues = 1;
        vkCtx->queue_family_decode_index = -1;
        vkCtx->nb_decode_queues = 0;

        vkCtx->lock_queue = &lockQueue;
        vkCtx->unlock_queue = &unlockQueue;

        if (av_hwdevice_ctx_init(hwDeviceCtx) < 0) {
            vkh::logger::log_err("av_hwdevice_ctx_init(Vulkan) failed");
            av_buffer_unref(&hwDeviceCtx);
            return false;
        }

        return true;
    }
    bool H264VideoEncoder::initFramesCtx() {
        hwFramesCtx = av_hwframe_ctx_alloc(hwDeviceCtx);

        if (!hwFramesCtx) {
            vkh::logger::log_err("av_hwframe_ctx_alloc failed");
            return false;
        }

        auto *fc = reinterpret_cast<AVHWFramesContext *>(hwFramesCtx->data);
        fc->format = AV_PIX_FMT_VULKAN;
        fc->sw_format = AV_PIX_FMT_NV12; // YUV 4:2:0
        fc->width = static_cast<int>(extent.width);
        fc->height = static_cast<int>(extent.height);
        fc->initial_pool_size = 0; // dynamic allocation

        auto *vkfc = static_cast<AVVulkanFramesContext *>(fc->hwctx);
        vkfc->tiling = VK_IMAGE_TILING_OPTIMAL;
        vkfc->usage =
                static_cast<VkImageUsageFlagBits>(VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR | VK_IMAGE_USAGE_STORAGE_BIT |
                                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

        if (const int r = av_hwframe_ctx_init(hwFramesCtx); r < 0) {
            ffmpegError("av_hwframe_ctx_init", r);
            av_buffer_unref(&hwFramesCtx);
            return false;
        }

        return true;
    }
    bool H264VideoEncoder::initFormat() {
        if (avformat_alloc_output_context2(&formatCtx, nullptr, "mp4", filename.c_str()) < 0) {
            vkh::logger::log_err("avformat_alloc_output_context2 failed");
            return false;
        }
        return true;
    }
    bool H264VideoEncoder::initEncoder() {
        const AVCodec *codec = avcodec_find_encoder_by_name("h264_vulkan");

        if (!codec) {
            vkh::logger::log_err("h264_vulkan is not available");
            return false;
        }

        codecCtx = avcodec_alloc_context3(codec);

        if (!codecCtx) {
            vkh::logger::log_err("avcodec_alloc_context3 failed");
            return false;
        }

        codecCtx->width = static_cast<int>(extent.width);
        codecCtx->height = static_cast<int>(extent.height);
        const auto fps = static_cast<int>(videoSettings.exportation.fps);

        codecCtx->time_base = AVRational{.num = 1, .den = fps};
        codecCtx->framerate = AVRational{.num = fps, .den = 1};
        codecCtx->pix_fmt = AV_PIX_FMT_VULKAN;
        codecCtx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);

        if (!codecCtx->hw_device_ctx) {
            vkh::logger::log_err("failed to reference Vulkan HW device");

            return false;
        }

        codecCtx->hw_frames_ctx = av_buffer_ref(hwFramesCtx);

        if (!codecCtx->hw_frames_ctx) {
            vkh::logger::log_err("failed to reference Vulkan HW frames");

            return false;
        }

        // configuring context
        codecCtx->bit_rate = static_cast<int64_t>(videoSettings.exportation.bitrate) << 20;
        codecCtx->gop_size = fps * 2;
        codecCtx->max_b_frames = 0;

        if (formatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (const int r = avcodec_open2(codecCtx, codec, nullptr); r < 0) {
            ffmpegError("avcodec_open2(h264_vulkan)", r);
            return false;
        }

        return true;
    }
    bool H264VideoEncoder::initStream() {
        stream = avformat_new_stream(formatCtx, nullptr);

        if (!stream) {
            vkh::logger::log_err("avformat_new_stream failed");
            return false;
        }

        stream->time_base = codecCtx->time_base;
        stream->avg_frame_rate = codecCtx->framerate;

        if (avcodec_parameters_from_context(stream->codecpar, codecCtx) < 0) {
            vkh::logger::log_err("avcodec_parameters_from_context failed");
            return false;
        }


        if (!(formatCtx->oformat->flags & AVFMT_NOFILE) &&
            avio_open(&formatCtx->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
            vkh::logger::log_err("avio_open failed");
            return false;
        }

        if (avformat_write_header(formatCtx, nullptr) < 0) {
            vkh::logger::log_err("avformat_write_header failed");
            return false;
        }

        return true;
    }
    void H264VideoEncoder::drainPackets() const {
        AVPacket *packet = av_packet_alloc();

        if (!packet)
            throw std::bad_alloc();

        while (true) {

            const int ret = avcodec_receive_packet(codecCtx, packet);

            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            }

            if (ret < 0) {
                av_packet_free(&packet);
                ffmpegError("avcodec_receive_packet", ret);
                return;
            }

            av_packet_rescale_ts(packet, codecCtx->time_base, stream->time_base);

            packet->stream_index = stream->index;

            if (av_interleaved_write_frame(formatCtx, packet) < 0) {
                av_packet_unref(packet);
                av_packet_free(&packet);
                vkh::logger::log_err("av_interleaved_write_frame failed");
                return;
            }

            av_packet_unref(packet);
        }

        av_packet_free(&packet);
    }

    void H264VideoEncoder::flushEncoder() const {
        if (!codecCtx)
            return;

        const int ret = avcodec_send_frame(codecCtx, nullptr);
        if (ret < 0 && ret != AVERROR_EOF) {
            return;
        }
        drainPackets();
    }
    std::string H264VideoEncoder::ffmpegErrorString(const int error) {
        std::array<char, AV_ERROR_MAX_STRING_SIZE> buffer{};
        av_strerror(error, buffer.data(), buffer.size());
        return std::string(buffer.data());
    }
    void H264VideoEncoder::ffmpegError(const char *operation, const int error) {
        vkh::logger::log_err("{} failed: {}", operation, ffmpegErrorString(error));
    }
    void H264VideoEncoder::init() {
        if (!initDevice())
            return;

        if (!initFramesCtx())
            return;

        if (!initFormat())
            return;

        if (!initEncoder())
            return;

        if (!initStream())
            return;

        initialized = true;
    }
    void H264VideoEncoder::cleanup() {
        if (codecCtx) {

            if (initialized) {
                flushEncoder();
            }

            avcodec_free_context(&codecCtx);
        }

        if (formatCtx) {

            if (initialized) {
                av_write_trailer(formatCtx);
            }

            if (formatCtx->pb && !(formatCtx->oformat->flags & AVFMT_NOFILE)) {
                avio_closep(&formatCtx->pb);
            }

            avformat_free_context(formatCtx);

            formatCtx = nullptr;
        }

        stream = nullptr;
        av_buffer_unref(&hwFramesCtx);
        av_buffer_unref(&hwDeviceCtx);
        initialized = false;
    }
} // namespace merutilm::rff2
