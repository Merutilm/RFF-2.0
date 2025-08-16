//
// Created by Merutilm on 2025-08-08.
//

#include "Application.hpp"

#include "../../vulkan_helper/configurator/GeneralPostProcessPipelineConfigurator.hpp"
#include "../../vulkan_helper/executor/RenderPassExecutor.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../../vulkan_helper/util/Presenter.hpp"
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
        createVulkanContext();
        createRenderScene();
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

        SendMessage(statusBar, SB_SETPARTS, Constants::Status::LENGTH, (LPARAM) rightEdges.data());
    }

    void Application::refreshStatusBar() const {
        for (int i = 0; i < Constants::Status::LENGTH; ++i) {
            SendMessage(statusBar, SB_SETTEXT, i, (LPARAM) TEXT(statusMessages[i].data()));
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

        statusHeight = 0;
        if (statusBar) {
            RECT statusRect;
            GetWindowRect(statusBar, &statusRect);
            statusHeight = statusRect.bottom - statusRect.top;
        }
    }

    void Application::createVulkanContext() {
        auto core = vkh::Core::createCore();
        core->createGraphicsContextForWindow(renderWindow, Constants::Win32::INIT_RENDER_SCENE_FPS,
                                             Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX);
        engine = std::make_unique<vkh::Engine>(std::move(core));
    }

    void Application::createRenderScene() {
        scene = std::make_unique<RenderScene>(*engine, renderWindow, &statusMessages);
    }

    void Application::setProcedure() {
        const HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW);

        vkh::GraphicsContextWindow &window = *engine->getCore().getWindowContext(
            Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).window;

        window.setListener(
            WM_GETMINMAXINFO, [](const vkh::GraphicsContextWindow &, HWND, WPARAM, const LPARAM lparam) {
                const auto min = reinterpret_cast<LPMINMAXINFO>(lparam);
                min->ptMinTrackSize.x = Constants::Win32::MIN_WINDOW_WIDTH;
                min->ptMinTrackSize.y = Constants::Win32::MIN_WINDOW_HEIGHT;
                return static_cast<LRESULT>(0);
            });
        window.setListener(WM_MOUSEMOVE, [hCursor](const vkh::GraphicsContextWindow &, HWND, WPARAM, LPARAM) {
            SetCursor(hCursor);
            return static_cast<LRESULT>(true);
        });
        window.setListener(WM_SIZING, [this](const vkh::GraphicsContextWindow &, HWND, WPARAM, LPARAM) {
            windowResizing = true;
            return static_cast<LRESULT>(0);
        });
        window.setListener(WM_SIZE, [this](const vkh::GraphicsContextWindow &, HWND, const WPARAM wparam, LPARAM) {
            if (wparam == SIZE_MAXIMIZED) {
                resolveWindowResizeEnd();
            }
            if (wparam == SIZE_RESTORED && !windowResizing) {
                resolveWindowResizeEnd();
            }
            return static_cast<LRESULT>(0);
        });

        window.setListener(WM_EXITSIZEMOVE, [this](const vkh::GraphicsContextWindow &, HWND, WPARAM, LPARAM) {
            if (windowResizing) {
                windowResizing = false;
                resolveWindowResizeEnd();
            }
            return static_cast<LRESULT>(0);
        });
        window.setListener(
            WM_INITMENUPOPUP, [this](const vkh::GraphicsContextWindow &, HWND, const WPARAM wparam, LPARAM) {
                const auto popup = reinterpret_cast<HMENU>(wparam);
                const int count = GetMenuItemCount(popup);
                for (int i = 0; i < count; ++i) {
                    //synchronize current settings
                    MENUITEMINFO info = {};
                    info.cbSize = sizeof(MENUITEMINFO);
                    info.fMask = MIIM_ID;
                    if (GetMenuItemInfo(popup, i, TRUE, &info)) {
                        if (const UINT id = info.wID;
                            settingsMenu->hasCheckbox(id)
                        ) {
                            const bool *ref = settingsMenu->getBool(*scene, id);
                            assert(ref != nullptr);
                            CheckMenuItem(popup, id, MF_BYCOMMAND | (*ref ? MF_CHECKED : MF_UNCHECKED));
                        }
                    }
                }
                return static_cast<LRESULT>(0);
            });
        window.setListener(WM_COMMAND, [this](const vkh::GraphicsContextWindow &, HWND, const WPARAM wparam, LPARAM) {
            const HMENU menu = GetMenu(masterWindow);
            if (const int menuID = LOWORD(wparam);
                settingsMenu->hasCheckbox(menuID)
            ) {
                bool *ref = settingsMenu->getBool(*scene, menuID);
                assert(ref != nullptr);
                *ref = !*ref;
                settingsMenu->executeAction(*scene, menuID);
                CheckMenuItem(menu, menuID, *ref ? MF_CHECKED : MF_UNCHECKED);
            } else {
                settingsMenu->executeAction(*scene, menuID);
            }
            return static_cast<LRESULT>(0);
        });
        window.setListener(WM_CLOSE, [this](const vkh::GraphicsContextWindow &, HWND, WPARAM, LPARAM) {
            DestroyWindow(masterWindow);
            return static_cast<LRESULT>(0);
        });
        window.setListener(WM_DESTROY, [](const vkh::GraphicsContextWindow &, HWND, WPARAM, LPARAM) {
            PostQuitMessage(0);
            return static_cast<LRESULT>(0);
        });

        window.appendRenderer([this] {
            drawFrame();
        });
    }

    void Application::resolveWindowResizeEnd() {
        RECT rect;
        GetClientRect(masterWindow, &rect);
        rect.bottom -= statusHeight;
        if (rect.bottom - rect.top > 0 || rect.right - rect.left > 0) {
            adjustClient(rect);
            scene->requestResize();
            scene->requestRecompute();
        }

        vkh::Core &core = engine->getCore();
        vkh::RenderContext &renderContext = engine->getRenderContext();
        if (core.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).window->isUnrenderable()) {
            return;
        }
        vkDeviceWaitIdle(core.getLogicalDevice().getLogicalDeviceHandle());

        vkh::Swapchain &swapchain = *core.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).
                swapchain;
        swapchain.recreate();
        renderContext.recreate(swapchain.populateSwapchainExtent());
        drawFrame();
    }

    void Application::drawFrame() {
        vkh::Core &core = engine->getCore();
        const vkh::RenderContext &renderContext = engine->getRenderContext();
        if (core.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).window->isUnrenderable()) {
            return;
        }
        changeFrameIndex();
        const vkh::Swapchain &swapchain = *core.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).
                swapchain;
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkFence currentFence = engine->getSyncObject().getFence(currentFrame);
        const VkSemaphore imageAvailableSemaphore = engine->getSyncObject().getImageAvailableSemaphore(currentFrame);
        const VkSwapchainKHR swapchainHandle = swapchain.getSwapchainHandle();


        vkWaitForFences(device, 1, &currentFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &currentFence);

        uint32_t imageIndex = 0;
        vkAcquireNextImageKHR(device, swapchainHandle, UINT64_MAX, imageAvailableSemaphore,
                              nullptr, &imageIndex);

        const auto extent = swapchain.populateSwapchainExtent();

        vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();

        for (const auto &shaderPrograms = scene->getShaderPrograms();
            const auto &shaderProgram: shaderPrograms) {
            shaderProgram->updateQueue(queue, currentFrame, imageIndex, extent.width, extent.height);
        }

        //TODO : Move to TimeDescConfigurator and invoke it from external class (Every Frame)
        using namespace SharedDescriptorTemplate;
        auto &desc = engine->getRepositories().getRepository<vkh::SharedDescriptorRepo>()->
                pick(vkh::DescriptorTemplate::from<DescTime>(),
                     engine->getRepositories().getDescriptorRequiresRepositoryContext());
        const auto &timeBinding = *desc.getDescriptorManager().get<
                    std::unique_ptr<
                        vkh::Uniform> >(DescTime::BINDING_UBO_TIME);
        timeBinding.getHostObject().set(DescTime::TARGET_TIME_CURRENT, Utilities::getCurrentTime());
        timeBinding.update(currentFrame);
        desc.queue(queue, currentFrame);
        //TODO_END


        vkh::DescriptorUpdater::write(device, queue);

        //COMMAND SCOPE START
        {
            const auto renderPassExecutor = vkh::RenderPassExecutor(*engine, renderContext, extent,
                                                                    currentFrame, imageIndex);

            const VkCommandBuffer cbh = engine->getCommandBuffer().getCommandBufferHandle(currentFrame);
            swapchain.matchViewportAndScissor(cbh);
            scene->render(cbh, currentFrame, extent);
        } // COMMAND SCOPE END

        vkh::Presenter::present(*engine, swapchainHandle, currentFrame, imageIndex);
    }

    void Application::changeFrameIndex() {
        ++currentFrame %= engine->getCore().getPhysicalDevice().getMaxFramesInFlight();
    }

    void Application::prepareWindow() const {
        ShowWindow(masterWindow, SW_SHOW);
        UpdateWindow(masterWindow);
        SetWindowLongPtr(masterWindow, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(engine->getCore().getWindowContext(
                                 Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).
                             window.get()));
        SetWindowLongPtr(renderWindow, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(scene.get()));
    }

    void Application::start() const {
        const vkh::GraphicsContextWindow &window = *engine->getCore().getWindowContext(
                    Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).
                window;
        window.start();
    }

    void Application::destroy() {
        vkDeviceWaitIdle(engine->getCore().getLogicalDevice().getLogicalDeviceHandle());
        scene = nullptr;
        vkh::GeneralPostProcessPipelineConfigurator::cleanup();
        engine = nullptr;
        settingsMenu = nullptr;
    }
}
