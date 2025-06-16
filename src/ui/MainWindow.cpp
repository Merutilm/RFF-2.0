#include <glad_wgl.h>
#include "MainWindow.h"

#include <iostream>
#include <windows.h>
#include "Constants.h"
#include <assert.h>
#include <commctrl.h>

#include "Win32GLContextLoader.h"
#include "SettingsMenu.h"


namespace merutilm::rff {
    LRESULT CALLBACK MainWindow::masterWindowProc(const HWND masterWindow, const UINT message, const WPARAM wParam,
                                                                 const LPARAM lParam) {
        auto &wnd = *reinterpret_cast<MainWindow *>(GetWindowLongPtr(masterWindow, GWLP_USERDATA));

        switch (message) {
            case WM_GETMINMAXINFO:
            {
                const auto min = reinterpret_cast<LPMINMAXINFO>(lParam);
                min->ptMinTrackSize.x = Constants::Win32::MIN_WINDOW_WIDTH;
                min->ptMinTrackSize.y = Constants::Win32::MIN_WINDOW_HEIGHT;
                return 0;
            }
            case WM_SIZE: {
                RECT rect;
                GetClientRect(masterWindow, &rect);
                rect.bottom -= wnd.statusHeight;
                wnd.adjustClient(rect);
                wnd.scene.requestResize();
                wnd.scene.requestRecompute();
                return 0;
            }
            case WM_INITMENUPOPUP: {
                const auto popup = reinterpret_cast<HMENU>(wParam);
                const int count = GetMenuItemCount(popup);
                for (int i = 0; i < count; ++i) { //synchronize current settings
                    MENUITEMINFO info = {};
                    info.cbSize = sizeof(MENUITEMINFO);
                    info.fMask = MIIM_ID;
                    if (GetMenuItemInfo(popup, i, TRUE, &info)) {
                        if (const UINT id = info.wID;
                            wnd.settingsMenu->hasCheckbox(id)
                            ) {
                            const bool* ref = wnd.settingsMenu->getBool(wnd.scene, id);
                            assert(ref != nullptr);
                            CheckMenuItem(popup, id, MF_BYCOMMAND | (*ref ? MF_CHECKED : MF_UNCHECKED));
                            }
                    }
                }
                return 0;
            }
            case WM_COMMAND: {
                const HMENU menu = GetMenu(masterWindow);
                if (const int menuID = LOWORD(wParam);
                    wnd.settingsMenu->hasCheckbox(menuID)
                ) {
                    bool *ref = wnd.settingsMenu->getBool(wnd.scene, menuID);
                    assert(ref != nullptr);
                    *ref = !*ref;
                    wnd.settingsMenu->executeAction(wnd.scene, menuID);
                    CheckMenuItem(menu, menuID, *ref ? MF_CHECKED : MF_UNCHECKED);
                } else {
                    wnd.settingsMenu->executeAction(wnd.scene, menuID);
                }
                return 0;
            }
            case WM_CLOSE: {
                wnd.running = false;
                return 0;
            }

            case WM_DESTROY: {
                PostQuitMessage(0);
                wglDeleteContext(wnd.context);
                wnd.context = nullptr;
                ReleaseDC(masterWindow, wnd.hdc);
                wnd.hdc = nullptr;
                return 0;
            }
            default:
                return DefWindowProc(masterWindow, message, wParam, lParam);
        }
    }

    LRESULT CALLBACK MainWindow::renderSceneProc(const HWND renderWindow, const UINT message, const WPARAM wParam,
                                                      const LPARAM lParam) {
        auto &wnd = *reinterpret_cast<MainWindow *>(GetWindowLongPtr(renderWindow, GWLP_USERDATA));
        wnd.scene.runAction(message, wParam);
        switch (message) {
            case WM_CLOSE: {
                DestroyWindow(renderWindow);
                return 0;
            }
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
            default: return DefWindowProc(renderWindow, message, wParam, lParam);
        }
    }

    MainWindow::MainWindow() : scene(RenderScene()) {
        initWindow();
    }

    void MainWindow::initMenu(const HMENU hMenubar) {
        settingsMenu = std::make_unique<SettingsMenu>(hMenubar);
    }


    void MainWindow::initWindow() {
        SetProcessDPIAware();

        const HMENU hMenubar = CreateMenu();
        initMenu(hMenubar);


        createMasterWindow(hMenubar);
        createRenderScene();
        createStatusBar();

        statusHeight = 0;
        if (statusBar) {
            RECT statusRect;
            GetWindowRect(statusBar, &statusRect);
            statusHeight = statusRect.bottom - statusRect.top;
        }

        running = true;

        setClientSize(Constants::Win32::INIT_RENDER_SCENE_WIDTH, Constants::Win32::INIT_RENDER_SCENE_HEIGHT);
        ShowWindow(masterWindow, SW_SHOW);
        UpdateWindow(masterWindow);
    }


    void MainWindow::setClientSize(const int width, const int height) const {
        const RECT rect = {0, 0, width, height};
        RECT adjusted = rect;
        AdjustWindowRect(&adjusted, WS_OVERLAPPEDWINDOW | WS_SYSMENU, true);

        SetWindowPos(masterWindow, nullptr, 0, 0, adjusted.right - adjusted.left,
                     adjusted.bottom - adjusted.top + statusHeight, SWP_NOMOVE | SWP_NOZORDER);
        adjustClient(rect);
    }

    void MainWindow::adjustClient(const RECT &rect) const {
        SetWindowPos(renderWindow, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
        SetWindowPos(statusBar, nullptr, 0, rect.bottom - rect.top, rect.right - rect.left, statusHeight, SWP_NOZORDER);

        auto rightEdges = std::array<int, Constants::Status::LENGTH>{};

        const int statusBarWidth = rect.right / Constants::Status::LENGTH;
        int rightEdge = statusBarWidth;
        for (int i = 0; i < Constants::Status::LENGTH; i++) {
            rightEdges[i] = rightEdge;
            rightEdge += statusBarWidth;
        }

        SendMessage(statusBar, SB_SETPARTS, Constants::Status::LENGTH, (LPARAM) rightEdges.data());
    }

    void MainWindow::refreshStatusBar() const {
        for (int i = 0; i < Constants::Status::LENGTH; ++i) {
            SendMessage(statusBar, SB_SETTEXT, i, (LPARAM) TEXT(statusMessages[i].data()));
        }
    }

    void MainWindow::createStatusBar() {
        statusBar = CreateWindowEx(
            0,
            STATUSCLASSNAME,
            nullptr,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_CLIPCHILDREN,
            0, 0, 0, 0,
            masterWindow,
            nullptr,
            nullptr,
            nullptr);
    }

    void MainWindow::createMasterWindow(const HMENU hMenubar) {

        masterWindow = CreateWindowEx(
            0,
            Constants::Win32::CLASS_MASTER_WINDOW,
            "RFF 2.0",
            WS_OVERLAPPEDWINDOW | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT, nullptr, hMenubar, nullptr, nullptr
        );

        SetWindowLongPtr(masterWindow, GWLP_USERDATA, (LONG_PTR) this);
    }

    void MainWindow::createRenderScene() {
        renderWindow = CreateWindowEx(
            0,
            Constants::Win32::CLASS_RENDER_SCENE,
            "",
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT, masterWindow, nullptr, nullptr, nullptr);

        if (!masterWindow) {
            std::cout << "Failed to create window!\n";
            return;
        }

        SetWindowLongPtr(renderWindow, GWLP_USERDATA, (LONG_PTR) this);
        hdc = GetDC(renderWindow);
        Win32GLContextLoader::createContext(hdc, &context);
    }

    void MainWindow::renderLoop() {
        scene.configure(renderWindow, hdc, context, &statusMessages);
        MSG msg;
        auto lastRender = std::chrono::high_resolution_clock::now();
        constexpr int ms = 1000 / Constants::Win32::INIT_RENDER_SCENE_FPS;

        while (running) {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            std::this_thread::sleep_until(lastRender + std::chrono::milliseconds(ms));

            if (scene.getCWRequest() != 0) {
                setClientSize(scene.getCWRequest(), scene.getCHRequest());
                scene.clientResizeRequestSolved();
                scene.requestResize();
                scene.requestRecompute();
            }

            if (auto currentTime = std::chrono::high_resolution_clock::now();
                currentTime - lastRender > std::chrono::milliseconds(ms)) {
                scene.renderGL();
                scene.getBackgroundThreads().removeAllEndedThread();
                refreshStatusBar();
                lastRender = currentTime;
                }
        }
        DestroyWindow(masterWindow);
    }


    RenderScene &MainWindow::getRenderScene() {
        return scene;
    }
}