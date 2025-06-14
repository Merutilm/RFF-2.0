//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include <filesystem>

#include "RFFVideoRenderScene.h"
#include "../parallel/BackgroundThreads.h"
#include "../settings/Settings.h"

class RFFVideoWindow {

    HWND videoWindow;
    HWND renderWindow;
    HWND bar;
    float barRatio = 0;
    std::string barText = "";
    HDC hdc;
    HGLRC context;
    RFFVideoRenderScene scene;

public:
    RFFVideoWindow(uint16_t width, uint16_t height);

    ~RFFVideoWindow();

    RFFVideoWindow(const RFFVideoWindow&) = delete;

    RFFVideoWindow& operator=(const RFFVideoWindow&) = delete;

    RFFVideoWindow(RFFVideoWindow&&) = delete;

    RFFVideoWindow& operator=(RFFVideoWindow&&) = delete;

    void setClientSize(int width, int height) const;

    static LRESULT CALLBACK videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static void createVideo(const Settings &settings, const std::filesystem::path &open, const std::filesystem::path &save);

    void messageLoop() const;
};
