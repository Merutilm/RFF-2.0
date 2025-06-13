//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include <filesystem>

#include "RFFVideoRenderScene.h"
#include "../settings/Settings.h"

class RFFVideoWindow {

    HWND videoWindow;
    HWND renderWindow;
    HWND bar;
    HDC hdc;
    HGLRC context;
    RFFVideoRenderScene scene;

public:
    RFFVideoWindow(uint16_t width, uint16_t height);

    void setClientSize(int width, int height) const;

    static LRESULT videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static void createVideo(const Settings &settings, const std::filesystem::path &open, const std::filesystem::path &save);
};
