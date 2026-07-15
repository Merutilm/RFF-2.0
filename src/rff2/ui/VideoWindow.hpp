//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <opencv2/videoio.hpp>


#include "../settings/Settings.h"
#include "RFFApplication.hpp"
#include "VideoBufferCache.hpp"
#include "VideoWindowRenderManager.hpp"
#include "vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::rff2 {


    class VideoWindow final : vkh::Handler{

        RFFApplication &app;
        std::unique_ptr<VideoWindowRenderManager> scene = nullptr;
        const int width;
        const int height;


    public:
        explicit VideoWindow(RFFApplication &app, int width, int height);

        ~VideoWindow() override;

        VideoWindow(const VideoWindow &) = delete;

        VideoWindow &operator=(const VideoWindow &) = delete;

        VideoWindow(VideoWindow &&) = delete;

        VideoWindow &operator=(VideoWindow &&) = delete;


        static void createVideo(RFFApplication &app, const std::filesystem::path &open,
                                const std::filesystem::path &save);

        static cv::Mat generateFrame(const VideoBufferCache &buffer, int imgWidth, bool showText);

    protected:
        void initScene(const VkExtent2D &videoExtent, const Settings &targetSettings);

        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::rff2