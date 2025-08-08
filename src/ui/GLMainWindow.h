//
// Created by Merutilm on 2025-05-07.
//
#pragma once

#include "GLRenderScene.h"
#include "SettingsMenu.h"
#include "SettingsWindow.h"

namespace merutilm::rff2 {
    class GLMainWindow {

        HDC hdc = nullptr;
        HGLRC context = nullptr;
        int statusHeight = 0;
        bool running = false;
        std::array<std::string, Constants::Status::LENGTH> statusMessages = {};
        HWND masterWindow = nullptr;
        HWND renderWindow = nullptr;
        HWND statusBar = nullptr;
        GLRenderScene scene;
        std::unique_ptr<SettingsMenu> settingsMenu = nullptr;

    public:
        GLMainWindow();

        ~GLMainWindow() = default;

        GLMainWindow(const GLMainWindow& other) = delete;

        GLMainWindow& operator=(const GLMainWindow& other) = delete;

        GLMainWindow(GLMainWindow&& other) noexcept = delete;

        GLMainWindow& operator=(GLMainWindow&& other) noexcept = delete;

        void initMenu(HMENU hMenubar);

        void initWindow();

        void setClientSize(int width, int height) const;

        void adjustClient(const RECT &rect) const;

        void refreshStatusBar() const;

        void createStatusBar();

        void createRenderScene();

        void createMasterWindow(HMENU hMenubar);

        void renderLoop();

        GLRenderScene &getRenderScene();

        static LRESULT masterWindowProc(HWND masterWindow, UINT message, WPARAM wParam, LPARAM lParam);

        static LRESULT renderSceneProc(HWND renderWindow, UINT message, WPARAM wParam, LPARAM lParam);

    };
}