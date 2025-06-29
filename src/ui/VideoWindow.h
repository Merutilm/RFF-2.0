//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include <filesystem>

#include "VideoRenderScene.h"
#include "../parallel/BackgroundThreads.h"
#include "../settings/Settings.h"

namespace merutilm::rff2 {

    class VideoWindow {

        HWND videoWindow;
        HWND renderWindow;
        HWND bar;
        float barRatio = 0;
        std::string barText = "";
        HDC hdc;
        HGLRC context;
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

        static void createVideo(const Settings &settings, const std::filesystem::path &open, const std::filesystem::path &save);

        static void messageLoop();
    };
}