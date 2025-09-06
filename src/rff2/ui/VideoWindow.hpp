//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "VideoRenderScene.hpp"
#include "../../vulkan_helper/core/vkh.hpp"
#include "../attr/Attribute.h"

namespace merutilm::rff2 {

    class VideoWindow {
        HWND videoWindow;
        HWND renderWindow;
        HWND bar;
        float barRatio = 0;
        std::wstring barText = L"";
        VideoRenderScene scene;

        
    public:
        VideoWindow(uint16_t width, uint16_t height);

        ~VideoWindow() = default;

        VideoWindow(const VideoWindow&) = delete;

        VideoWindow& operator=(const VideoWindow&) = delete;

        VideoWindow(VideoWindow&&) = delete;

        VideoWindow& operator=(VideoWindow&&) = delete;

        void setClientSize(int width, int height) const;

        static LRESULT CALLBACK videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        static void createVideo(const Attribute &attr, const std::filesystem::path &open, const std::filesystem::path &save);

        static void messageLoop();
    };

}