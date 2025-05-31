//
// Created by Merutilm on 2025-05-07.
//
#pragma once

#include "RFFRenderScene.h"
#include "RFFSettingsMenu.h"
#include "RFFSettingsWindow.h"

class RFFMasterWindow {

    std::unique_ptr<RFFSettingsMenu> settingsMenu = nullptr;
    HDC hdc;
    HGLRC context;
    int statusHeight = 0;
    bool running = false;
    std::array<std::string, RFF::Status::LENGTH> statusMessages = {};
    HWND masterWindow = nullptr;
    HWND renderWindow = nullptr;
    HWND statusBar;
    RFFRenderScene renderer;

    static LRESULT masterWindowProc(HWND masterWindow, UINT message, WPARAM wParam, LPARAM lParam);

    static LRESULT renderSceneProc(HWND renderWindow, UINT message, WPARAM wParam, LPARAM lParam);

public:
    RFFMasterWindow();

    ~RFFMasterWindow() = default;

    RFFMasterWindow(const RFFMasterWindow& other) = delete;

    RFFMasterWindow& operator=(const RFFMasterWindow& other) = delete;

    RFFMasterWindow(RFFMasterWindow&& other) noexcept = delete;

    RFFMasterWindow& operator=(RFFMasterWindow&& other) noexcept = delete;

    void initMenu(HMENU hMenubar);

    void initWindow();

    void setClientSize(int width, int height) const;

    void adjustClient(const RECT &rect) const;

    void refreshStatusBar() const;

    void createStatusBar();

    void createRenderScene();

    void createMasterWindow(HMENU hMenubar);

    void renderLoop();

    RFFRenderScene &getRenderScene();

};


