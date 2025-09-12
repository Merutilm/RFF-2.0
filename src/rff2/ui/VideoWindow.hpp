//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "VideoRenderScene.hpp"
#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../attr/Attribute.h"

namespace merutilm::rff2 {

    class VideoWindow final : public vkh::EngineHandler{

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

        VideoWindow(const VideoWindow&) = delete;

        VideoWindow& operator=(const VideoWindow&) = delete;

        VideoWindow(VideoWindow&&) = delete;

        VideoWindow& operator=(VideoWindow&&) = delete;

        static LRESULT CALLBACK videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        static void createVideo(vkh::EngineRef engine, const Attribute &attr, const std::filesystem::path &open, const std::filesystem::path &save);

        static void messageLoop();

    private:

        void setClientSize(int width, int height) const;

        void createScene(const VkExtent2D &videoExtent, const Attribute &targetAttribute);

        void init() override;

        void destroy() override;
    };

}