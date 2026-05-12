//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <opencv2/videoio.hpp>


#include "../settings/Settings.h"
#include "VideoWindowRenderManager.hpp"
#include "vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::rff2 {


    class VideoWindow final : public vkh::EngineHandler {

        vkh::SharedResource &sr;
        HWND bar = nullptr;
        float barRatio = 0;
        std::wstring barText = L"";
        std::unique_ptr<VideoWindowRenderManager> scene = nullptr;
        const int width;
        const int height;


    public:
        explicit VideoWindow(vkh::Engine &engine, vkh::SharedResource &sr, int width, int height);

        ~VideoWindow() override;

        VideoWindow(const VideoWindow &) = delete;

        VideoWindow &operator=(const VideoWindow &) = delete;

        VideoWindow(VideoWindow &&) = delete;

        VideoWindow &operator=(VideoWindow &&) = delete;

        void addListeners() const;

        static void createVideo(vkh::Engine &engine, vkh::SharedResource &sr,const Settings &settings, const std::filesystem::path &open,
                                const std::filesystem::path &save);

        static cv::Mat generateFrame(const VideoBufferCache &buffer, int imgWidth, bool showText);

    private:

        void initScene(const VkExtent2D &videoExtent, const Settings &targetSettings);
        HWND getNativeWindowHandle() const;

        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::rff2
