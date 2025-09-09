//
// Created by Merutilm on 2025-08-08.
//

#include "Application.hpp"

#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    Application::Application() {
        Application::init();
    }

    Application::~Application() {
        Application::destroy();
    }


    void Application::init() {
        initWindow();
    }


    void Application::initWindow() {
        SetProcessDPIAware();
        const HMENU hMenubar = initMenu();
        createMasterWindow(hMenubar);
        createRenderWindow();
        createStatusBar();
        setClientSize(Constants::Win32::INIT_RENDER_SCENE_WIDTH, Constants::Win32::INIT_RENDER_SCENE_HEIGHT);
        createScene();
        prepareWindow();
        setProcedure();
    }

    HMENU Application::initMenu() {
        const HMENU hMenubar = CreateMenu();
        settingsMenu = std::make_unique<SettingsMenu>(hMenubar);
        return hMenubar;
    }


    void Application::setClientSize(const int width, const int height) const {
        const RECT rect = {0, 0, width, height};
        RECT adjusted = rect;
        AdjustWindowRect(&adjusted, WS_OVERLAPPEDWINDOW | WS_SYSMENU, true);

        SetWindowPos(masterWindow, nullptr, 0, 0, adjusted.right - adjusted.left,
                     adjusted.bottom - adjusted.top + statusHeight, SWP_NOMOVE | SWP_NOZORDER);
        adjustClient(rect);
    }

    void Application::adjustClient(const RECT &rect) const {
        SetWindowPos(renderWindow, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
        SetWindowPos(statusBar, nullptr, 0, rect.bottom - rect.top, rect.right - rect.left, statusHeight, SWP_NOZORDER);

        auto rightEdges = std::array<int, Constants::Status::LENGTH>{};

        const int statusBarWidth = rect.right / Constants::Status::LENGTH;
        int rightEdge = statusBarWidth;
        for (int i = 0; i < Constants::Status::LENGTH; i++) {
            rightEdges[i] = rightEdge;
            rightEdge += statusBarWidth;
        }

        SendMessageW(statusBar, SB_SETPARTS, Constants::Status::LENGTH, (LPARAM) rightEdges.data());
    }

    void Application::refreshStatusBar() const {
        for (int i = 0; i < Constants::Status::LENGTH; ++i) {
            SendMessageW(statusBar, SB_SETTEXTW, i, reinterpret_cast<LPARAM>(statusMessages[i].data()));
        }
    }

    void Application::createMasterWindow(const HMENU hMenubar) {
        masterWindow = CreateWindowExW(
            0,
            Constants::Win32::CLASS_MASTER_WINDOW,
            L"RFF 2.0",
            WS_OVERLAPPEDWINDOW | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT, nullptr, hMenubar, nullptr, nullptr
        );

        if (!masterWindow) {
            vkh::logger::log_err("Failed to create window!\n");
        }
    }

    void Application::createRenderWindow() {
        renderWindow = CreateWindowExW(
            0,
            Constants::Win32::CLASS_VK_RENDER_SCENE,
            L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT, masterWindow, nullptr, nullptr, nullptr);

        if (!renderWindow) {
            vkh::logger::log_err("Failed to create window!\n");
        }
    }

    void Application::createStatusBar() {
        statusBar = CreateWindowExW(
            0,
            STATUSCLASSNAMEW,
            nullptr,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_CLIPCHILDREN,
            0, 0, 0, 0,
            masterWindow,
            nullptr,
            nullptr,
            nullptr);

        statusHeight = 0;
        if (statusBar) {
            RECT statusRect;
            GetWindowRect(statusBar, &statusRect);
            statusHeight = statusRect.bottom - statusRect.top;
        }
    }

    void Application::createScene() {
        auto core = vkh::factory::create<vkh::Core>();
        engine = vkh::factory::create<vkh::Engine>(std::move(core));
        wc = engine->attachWindowContext(renderWindow, Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX);
        scene = std::make_unique<RenderScene>(*engine, *wc, &statusMessages);
    }

    void Application::setProcedure() {
        const HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW);

        vkh::GraphicsContextWindowRef window = wc->getWindow();

        window.setListener(
            WM_GETMINMAXINFO, [](vkh::GraphicsContextWindowRef, HWND, WPARAM, const LPARAM lparam) {
                const auto min = reinterpret_cast<LPMINMAXINFO>(lparam);
                min->ptMinTrackSize.x = Constants::Win32::MIN_WINDOW_WIDTH;
                min->ptMinTrackSize.y = Constants::Win32::MIN_WINDOW_HEIGHT;
                return static_cast<LRESULT>(0);
            });
        window.setListener(WM_MOUSEMOVE, [hCursor](vkh::GraphicsContextWindowRef, HWND, WPARAM, LPARAM) {
            SetCursor(hCursor);
            return static_cast<LRESULT>(true);
        });
        window.setListener(WM_SIZING, [this](vkh::GraphicsContextWindowRef, HWND, WPARAM, LPARAM) {
            windowResizing = true;
            return static_cast<LRESULT>(0);
        });
        window.setListener(WM_SIZE, [this](vkh::GraphicsContextWindowRef, HWND, const WPARAM wparam, LPARAM) {
            if (wparam == SIZE_MAXIMIZED) {
                resolveWindowResizeEnd();
            }
            if (wparam == SIZE_RESTORED && !windowResizing) {
                resolveWindowResizeEnd();
            }
            return static_cast<LRESULT>(0);
        });

        window.setListener(WM_EXITSIZEMOVE, [this](vkh::GraphicsContextWindowRef, HWND, WPARAM, LPARAM) {
            if (windowResizing) {
                windowResizing = false;
                resolveWindowResizeEnd();
            }
            return static_cast<LRESULT>(0);
        });
        window.setListener(
            WM_INITMENUPOPUP, [this](vkh::GraphicsContextWindowRef, HWND, const WPARAM wparam, LPARAM) {
                const auto popup = reinterpret_cast<HMENU>(wparam);
                const int count = GetMenuItemCount(popup);
                for (int i = 0; i < count; ++i) {
                    //synchronize current attr
                    MENUITEMINFO info = {};
                    info.cbSize = sizeof(MENUITEMINFO);
                    info.fMask = MIIM_ID;
                    if (GetMenuItemInfo(popup, i, TRUE, &info)) {
                        if (const UINT id = info.wID;
                            settingsMenu->hasCheckbox(id)
                        ) {
                            const bool *ref = settingsMenu->getBool(*scene, id, false);
                            if (ref == nullptr) {
                                throw vkh::exception_invalid_state("checkbox bool cannot be null");
                            }
                            CheckMenuItem(popup, id, MF_BYCOMMAND | (*ref ? MF_CHECKED : MF_UNCHECKED));
                        }
                    }
                }
                return static_cast<LRESULT>(0);
            });
        window.setListener(WM_COMMAND, [this](vkh::GraphicsContextWindowRef, HWND, const WPARAM wparam, LPARAM) {
            const HMENU menu = GetMenu(masterWindow);
            if (const int menuID = LOWORD(wparam);
                settingsMenu->hasCheckbox(menuID)
            ) {
                bool *ref = settingsMenu->getBool(*scene, menuID, true);
                if (ref == nullptr) {
                    throw vkh::exception_invalid_state("checkbox bool cannot be null");
                }
                *ref = !*ref;
                settingsMenu->executeAction(*scene, menuID);
                CheckMenuItem(menu, menuID, *ref ? MF_CHECKED : MF_UNCHECKED);
            } else {
                settingsMenu->executeAction(*scene, menuID);
            }
            return static_cast<LRESULT>(0);
        });
        window.setListener(WM_CLOSE, [this](vkh::GraphicsContextWindowRef, HWND, WPARAM, LPARAM) {
            DestroyWindow(masterWindow);
            return static_cast<LRESULT>(0);
        });
        window.setListener(WM_DESTROY, [](vkh::GraphicsContextWindowRef, HWND, WPARAM, LPARAM) {
            PostQuitMessage(0);
            return static_cast<LRESULT>(0);
        });

        window.appendRenderer([this] {
            resolveWNDRequest();
            scene->render();
            refreshStatusBar();
        });
    }

    void Application::resolveWindowResizeEnd() const {
        RECT rect;
        GetClientRect(masterWindow, &rect);
        rect.bottom -= statusHeight;
        if (rect.bottom - rect.top > 0 || rect.right - rect.left > 0) {
            adjustClient(rect);
            scene->resolveWindowResizeEnd();
            scene->getRequests().requestResize();
            scene->getRequests().requestRecompute();
        }
    }

    void Application::resolveWNDRequest() const {
        if (scene->getWndCWRequest() != 0) {
            setClientSize(scene->getWndCWRequest(), scene->getWndCHRequest());
            scene->wndClientSizeRequestSolved();
        }
        if (scene->isFPSRequested() != 0) {
            engine->getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).getWindow().setFramerate(
                scene->getAttribute().render.fps);
            scene->wndFPSRequestSolved();
        }
    }


    void Application::prepareWindow() const {
        ShowWindow(masterWindow, SW_SHOW);
        UpdateWindow(masterWindow);
        SetWindowLongPtr(masterWindow, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(&wc->getWindow()));
        SetWindowLongPtr(renderWindow, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(scene.get()));
    }

    void Application::start() const {
        wc->getWindow().start();
    }

    void Application::destroy() {
        engine->getCore().getLogicalDevice().waitDeviceIdle();
        scene = nullptr;
        vkh::GeneralPostProcessGraphicsPipelineConfigurator::cleanup();
        engine = nullptr;
        settingsMenu = nullptr;
    }
}
