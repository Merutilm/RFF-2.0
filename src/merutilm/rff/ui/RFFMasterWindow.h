//
// Created by Merutilm on 2025-05-07.
//
#pragma once
#include <Windows.h>

#include "RFFRenderScene.h"
#include "../settings/Settings.h"


class RFFMasterWindow {

    RFFRenderScene renderer;
    HWND masterWindow;
    HWND renderWindow;
    HWND statusBar;
    HDC hdc;
    HGLRC context;
    int statusHeight = 0;
    bool running = false;
    std::array<std::string, RFFConstants::Status::LENGTH> statusMessages = {};

    static LRESULT WndMasterProc(HWND masterWindow, UINT message, WPARAM wParam, LPARAM lParam);

    static LRESULT WndRendererProc(HWND renderWindow, UINT message, WPARAM wParam, LPARAM lParam);

public:
    RFFMasterWindow();

    ~RFFMasterWindow() = default;

    RFFMasterWindow(const RFFMasterWindow& other) = delete;

    RFFMasterWindow& operator=(const RFFMasterWindow& other) = delete;

    RFFMasterWindow(RFFMasterWindow&& other) noexcept = delete;

    RFFMasterWindow& operator=(RFFMasterWindow&& other) noexcept = delete;

    static void initMenu(HMENU hMenubar);

    static UINT_PTR castMenu(HMENU menu);

    void initWindow();

    void initClientSize(int width, int height) const;

    void adjustClient(const RECT &rect) const;

    void refreshStatusBar() const;

    void createStatusBar();

    void createRenderWindow();

    void createMasterWindow(DWORD style, HMENU hMenubar);

    void renderLoop();

    void destroy() const;

    void exit();

};


