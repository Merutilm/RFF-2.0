//
// Created by Merutilm on 2025-09-06.
//

#pragma once


#include "../settings/Settings.h"
#include "RFF2.hpp"
#include "VideoBufferCache.hpp"
#include "VideoWindowRenderManager.hpp"

namespace merutilm::rff2 {


    class VideoWindow final : vkh::Handler{

        RFF2 &app;
        std::unique_ptr<VideoWindowRenderManager> scene = nullptr;
        const int width;
        const int height;


    public:
        explicit VideoWindow(RFF2 &app, int width, int height);

        ~VideoWindow() override;

        VideoWindow(const VideoWindow &) = delete;

        VideoWindow &operator=(const VideoWindow &) = delete;

        VideoWindow(VideoWindow &&) = delete;

        VideoWindow &operator=(VideoWindow &&) = delete;


        static void createVideo(RFF2 &app, const std::filesystem::path &open,
                                const std::filesystem::path &save, const Settings &settingsClone);

        static cv::Mat generateFrame(const VideoBufferCache &buffer, int imgWidth, bool showText);

    protected:
        void initScene(const VkExtent2D &videoExtent, const Settings &targetSettings);

        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::rff2