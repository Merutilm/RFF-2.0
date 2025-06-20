//
// Created by Merutilm on 2025-05-07.
//
#pragma once

#include "RenderScene.h"
#include "SettingsMenu.h"
#include "SettingsWindow.h"

namespace merutilm::rff {
    class MainWindow {

        HDC hdc;
        HGLRC context;
        int statusHeight = 0;
        bool running = false;
        std::array<std::string, Constants::Status::LENGTH> statusMessages = {};
        HWND masterWindow = nullptr;
        HWND renderWindow = nullptr;
        HWND statusBar;
        RenderScene scene;
        std::unique_ptr<SettingsMenu> settingsMenu = nullptr;

    public:
        MainWindow();

        ~MainWindow() = default;

        MainWindow(const MainWindow& other) = delete;

        MainWindow& operator=(const MainWindow& other) = delete;

        MainWindow(MainWindow&& other) noexcept = delete;

        MainWindow& operator=(MainWindow&& other) noexcept = delete;

        void initMenu(HMENU hMenubar);

        void initWindow();

        void setClientSize(int width, int height) const;

        void adjustClient(const RECT &rect) const;

        void refreshStatusBar() const;

        void createStatusBar();

        void createRenderScene();

        void createMasterWindow(HMENU hMenubar);

        void renderLoop();

        RenderScene &getRenderScene();

        static LRESULT masterWindowProc(HWND masterWindow, UINT message, WPARAM wParam, LPARAM lParam);

        static LRESULT renderSceneProc(HWND renderWindow, UINT message, WPARAM wParam, LPARAM lParam);

    };
}