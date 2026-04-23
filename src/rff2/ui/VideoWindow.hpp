//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <cuda_runtime.h>
#include <opencv2/videoio.hpp>
#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../settings/Settings.h"
#include "VideoRenderScene.hpp"
#include "FFmpegContext.hpp"

namespace merutilm::rff2 {

    class VideoWindow final : public vkh::EngineHandler {

        HWND videoWindow = nullptr;
        HWND renderWindow = nullptr;
        HWND bar = nullptr;
        float barRatio = 0;
        std::wstring barText = L"";
        std::unique_ptr<VideoRenderScene> scene = nullptr;
        const int width;
        const int height;


    public:
        explicit VideoWindow(vkh::EngineRef engine, int width, int height);

        ~VideoWindow() override;

        VideoWindow(const VideoWindow &) = delete;

        VideoWindow &operator=(const VideoWindow &) = delete;

        VideoWindow(VideoWindow &&) = delete;

        VideoWindow &operator=(VideoWindow &&) = delete;

        static LRESULT CALLBACK videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        static cv::VideoWriter createWriter(const std::string &filename, int width, int height, double fps,
                                            uint32_t bitrate);

        static void createVideo(vkh::EngineRef engine, const Settings &settings, const std::filesystem::path &open,
                                const std::filesystem::path &save);

        static void messageLoop();

        void renderOnce(const FFmpegContext &ffmpegContext, uint32_t frameNum, FILE *file) const;


        static int encode(AVCodecContext *ctx, const AVFrame *frame, FILE *file);

    private:
        void setClientSize(int width, int height) const;

        void createScene(const VkExtent2D &videoExtent, const Settings &targetSettings);

        void init() override;

        void destroy() override;
    };

} // namespace merutilm::rff2
