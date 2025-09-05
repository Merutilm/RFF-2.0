//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include <filesystem>

#include "GLVideoRenderScene.h"
#include "../parallel/BackgroundThreads.h"
#include "../attr/Attribute.h"

namespace merutilm::rff2 {

    class GLVideoWindow {

        HWND videoWindow;
        HWND renderWindow;
        HWND bar;
        float barRatio = 0;
        std::string barText = "";
        HDC hdc;
        HGLRC context;
        GLVideoRenderScene scene;

    public:
        GLVideoWindow(uint16_t width, uint16_t height);

        ~GLVideoWindow() = default;

        GLVideoWindow(const GLVideoWindow&) = delete;

        GLVideoWindow& operator=(const GLVideoWindow&) = delete;

        GLVideoWindow(GLVideoWindow&&) = delete;

        GLVideoWindow& operator=(GLVideoWindow&&) = delete;

        void setClientSize(int width, int height) const;

        static LRESULT CALLBACK videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        static void createVideo(const Attribute &settings, const std::filesystem::path &open, const std::filesystem::path &save);

        static void messageLoop();
    };
}