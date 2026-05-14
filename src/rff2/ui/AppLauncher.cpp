//
// Created by Merutilm on 2025-08-08.
//

#include "AppLauncher.hpp"

#include "vulkan_helper/engine/window/win/NativeMenu.hpp"
#include "vulkan_helper/engine/window/win/NativeWindow.hpp"

namespace merutilm::rff2 {
    AppLauncher::AppLauncher() { AppLauncher::init(); }

    AppLauncher::~AppLauncher() { AppLauncher::cleanup(); }


    void AppLauncher::init() { initWindow(); }


    void AppLauncher::initWindow() {
        vkh::WindowInitializerSettings wic = {.framerate = Constants::Win32::INIT_RENDER_SCENE_FPS,
                                              .name = L"RFF 2.0",
                                              .icon = L"#1",
                                              .widthInfo = {.min = Constants::Win32::MIN_WINDOW_WIDTH,
                                                            .max = INT_MAX,
                                                            .first = Constants::Win32::INIT_RENDER_SCENE_WIDTH},
                                              .heightInfo = {.min = Constants::Win32::MIN_WINDOW_HEIGHT,
                                                             .max = INT_MAX,
                                                             .first = Constants::Win32::INIT_RENDER_SCENE_HEIGHT},
                                              .useMenubar = true,
                                              .resizable = true,
                                              .filedrop = false,
                                              .paddings = {.top = 0, .left = 0, .bottom = 0, .right = 0}};

        wc = &engine->attachWindowContext(std::move(wic), Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX);

        createStatusBar();
        createRenderManager();
        initMenu();
        setProcedure();
    }

    void AppLauncher::initMenu() const { SettingsMenuGenerator::configure(*wc->getWindow(), *arm); }


    void AppLauncher::refreshStatusBar() const {
        for (int i = 0; i < Constants::Status::LENGTH; ++i) {
            SendMessageW(statusBar, SB_SETTEXTW, i, reinterpret_cast<LPARAM>(statusMessages[i].data()));
        }
    }


    void AppLauncher::createStatusBar() {
        statusBar =
                CreateWindowExW(0, STATUSCLASSNAMEW, nullptr, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_CLIPCHILDREN,
                                0, 0, 0, 0, getNativeWindow().getMainWindow(), nullptr, nullptr, nullptr);

        statusHeight = 0;
        if (statusBar) {
            RECT statusRect;
            GetWindowRect(statusBar, &statusRect);
            statusHeight = statusRect.bottom - statusRect.top;
            wc->getWindow()->initializerSettings.paddings.bottom = statusHeight;
            wc->getWindow()->setRenderWindowSizeWithClientAdjustment(Constants::Win32::INIT_RENDER_SCENE_WIDTH,
                                                                     Constants::Win32::INIT_RENDER_SCENE_HEIGHT);
        }
    }


    void AppLauncher::createRenderManager() {
        sharedResource.emplace(*wc);
        arm = std::make_unique<AppRenderManager>(*engine, *wc, *sharedResource, &statusMessages);
    }

    void AppLauncher::setProcedure() const {
        auto &eventSystem = wc->getWindow()->eventSystem;
        eventSystem.window.onMaximize.add([this] { arm->resolveWindowResizeEnd(); });
        eventSystem.window.onRestore.add([this] { arm->resolveWindowResizeEnd(); });
        eventSystem.resize.onResizeEnd.add([this](const int w, const int h) {
            SetWindowPos(statusBar, nullptr, 0, h, w, statusHeight, SWP_NOZORDER);

            auto rightEdges = std::array<int, Constants::Status::LENGTH>{};

            const int statusBarWidth = w / Constants::Status::LENGTH;
            int rightEdge = statusBarWidth;
            for (int i = 0; i < Constants::Status::LENGTH; i++) {
                rightEdges[i] = rightEdge;
                rightEdge += statusBarWidth;
            }

            SendMessageW(statusBar, SB_SETPARTS, Constants::Status::LENGTH, (LPARAM) rightEdges.data());
            arm->resolveWindowResizeEnd();
        });

        eventSystem.applicationLifecycle.onUpdate.add([this] {
            arm->render();
            refreshStatusBar();
        });
    }


    void AppLauncher::start() const { wc->getWindow()->start(); }

    void AppLauncher::cleanup() {
        engine->getCore().getLogicalDevice().waitDeviceIdle();
        arm = nullptr;
    }
} // namespace merutilm::rff2
