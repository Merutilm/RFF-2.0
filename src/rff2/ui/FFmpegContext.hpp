//
// Created by Merutilm on 2026-04-23.
//

#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
}

#include "nppi_color_conversion.h"

namespace merutilm::rff2 {
    struct FFmpegContext {

        AVFrame *frame;
        AVCodecContext *codecCtx;
        AVBufferRef *hwDeviceCtxBufferRef;
        AVBufferRef *hwFramesCtxBufferRef;
        NppStreamContext streamContext;

        static bool tryInit(uint32_t width, uint32_t height, uint32_t bitrate, int32_t fps, FFmpegContext *ctx);

        void freeContext() const;

    private:
        static void freeContext(AVBufferRef *hwDeviceCtxBufferRef, AVBufferRef *hwFramesCtxBufferRef,
                                AVCodecContext *codecCtx, AVFrame *frame);
    };

    // DEFINITION OF FFmpegContext

    inline bool FFmpegContext::tryInit(const uint32_t width, const uint32_t height, const uint32_t bitrate,
                                       const int32_t fps, FFmpegContext *ctx) {
        // codec
        const AVCodec *codec = avcodec_find_encoder_by_name("h264_nvenc");
        if (!codec)
            return false;

        // init hw device context
        AVBufferRef *hwDeviceCtxBufferRef = av_hwdevice_ctx_alloc(AV_HWDEVICE_TYPE_CUDA);
        if (av_hwdevice_ctx_init(hwDeviceCtxBufferRef) < 0) {
            freeContext(hwDeviceCtxBufferRef, nullptr, nullptr, nullptr);
            return false;
        }
        // init hw frame context
        AVBufferRef *hwFramesCtxBufferRef = av_hwframe_ctx_alloc(hwDeviceCtxBufferRef);
        const auto hwFrameCtx = reinterpret_cast<AVHWFramesContext *>(hwFramesCtxBufferRef->data);

        hwFrameCtx->format = AV_PIX_FMT_CUDA;
        hwFrameCtx->sw_format = AV_PIX_FMT_NV12;
        hwFrameCtx->width = width;
        hwFrameCtx->height = height;
        if (av_hwframe_ctx_init(hwFramesCtxBufferRef) < 0) {
            freeContext(hwDeviceCtxBufferRef, hwFramesCtxBufferRef, nullptr, nullptr);
            return false;
        }
        // init the codec context
        AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
        codecCtx->width = width;
        codecCtx->height = height;
        codecCtx->time_base = {1, fps};
        codecCtx->framerate = {fps, 1};
        codecCtx->bit_rate = bitrate * 1000;
        codecCtx->hw_device_ctx = hwDeviceCtxBufferRef;
        codecCtx->hw_frames_ctx = hwFramesCtxBufferRef;
        codecCtx->pix_fmt = AV_PIX_FMT_CUDA;
        codecCtx->gop_size = fps;
        codecCtx->max_b_frames = 0;
        codecCtx->refs = 1;
        if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
            freeContext(hwDeviceCtxBufferRef, hwFramesCtxBufferRef, codecCtx, nullptr);
            return false;
        }

        // init frame

        AVFrame *frame = av_frame_alloc();
        frame->format = AV_PIX_FMT_CUDA;
        frame->width = width;
        frame->height = height;

        if (av_hwframe_get_buffer(codecCtx->hw_frames_ctx, frame, 0) < 0) {
            freeContext(hwDeviceCtxBufferRef, hwFramesCtxBufferRef, codecCtx, frame);
            return false;
        }

        if (av_frame_make_writable(frame) < 0) {
            freeContext(hwDeviceCtxBufferRef, hwFramesCtxBufferRef, codecCtx, frame);
            return false;
        }

        NppStreamContext streamContext{};

        cudaGetDevice(&streamContext.nCudaDeviceId);
        cudaDeviceGetAttribute(&streamContext.nMultiProcessorCount, cudaDevAttrMultiProcessorCount,
                               streamContext.nCudaDeviceId);
        cudaDeviceGetAttribute(&streamContext.nMaxThreadsPerMultiProcessor, cudaDevAttrMaxThreadsPerMultiProcessor,
                               streamContext.nCudaDeviceId);
        cudaDeviceGetAttribute(&streamContext.nMaxThreadsPerBlock, cudaDevAttrMaxThreadsPerBlock,
                               streamContext.nCudaDeviceId);
        cudaDeviceGetAttribute(reinterpret_cast<int *>(&streamContext.nSharedMemPerBlock),
                               cudaDevAttrMaxSharedMemoryPerBlock, streamContext.nCudaDeviceId);
        cudaDeviceGetAttribute(&streamContext.nCudaDevAttrComputeCapabilityMajor, cudaDevAttrComputeCapabilityMajor,
                               streamContext.nCudaDeviceId);
        cudaDeviceGetAttribute(&streamContext.nCudaDevAttrComputeCapabilityMinor, cudaDevAttrComputeCapabilityMinor,
                               streamContext.nCudaDeviceId);
        cudaStreamGetFlags(streamContext.hStream, &streamContext.nStreamFlags);
        streamContext.nReserved0 = 0;

        if (ctx)
            *ctx = {.frame = frame,
                    .codecCtx = codecCtx,
                    .hwDeviceCtxBufferRef = hwDeviceCtxBufferRef,
                    .hwFramesCtxBufferRef = hwFramesCtxBufferRef,
                    .streamContext = std::move(streamContext)};
        return true;
    }

    inline void FFmpegContext::freeContext() const {
        freeContext(hwDeviceCtxBufferRef, hwFramesCtxBufferRef, codecCtx, frame);
    }

    inline void FFmpegContext::freeContext(AVBufferRef *hwDeviceCtxBufferRef, AVBufferRef *hwFramesCtxBufferRef,
                                           AVCodecContext *codecCtx, AVFrame *frame) {
        if (frame)
            av_frame_free(&frame);
        if (codecCtx)
            avcodec_free_context(&codecCtx);
        if (hwFramesCtxBufferRef)
            av_buffer_unref(&hwFramesCtxBufferRef);
        if (hwDeviceCtxBufferRef)
            av_buffer_unref(&hwDeviceCtxBufferRef);
    }
} // namespace merutilm::rff2
