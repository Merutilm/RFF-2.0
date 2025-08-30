//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include "RenderScene.hpp"
#include "SettingsMenu.hpp"
#include "../../vulkan_helper/impl/Engine.hpp"

namespace merutilm::rff2 {

    class Application final : public vkh::Handler {
        int statusHeight = 0;
        std::array<std::string, Constants::Status::LENGTH> statusMessages = {};
        HWND masterWindow = nullptr;
        HWND renderWindow = nullptr;
        HWND statusBar = nullptr;
        std::unique_ptr<RenderScene> scene = nullptr;
        std::unique_ptr<SettingsMenu> settingsMenu = nullptr;
        vkh::Engine engine = nullptr;
        uint32_t frameIndex = 0;
        bool windowResizing = false;

    public:
        explicit Application();

        ~Application() override;

        Application(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(const Application &) = delete;

        Application &operator=(Application &&) = delete;

        vkh::EngineRef getEngine() const { return *engine; }

        void start() const;

    private:
        void init() override;

        void initWindow();

        HMENU initMenu();

        void setClientSize(int width, int height) const;

        void adjustClient(const RECT &rect) const;

        void refreshStatusBar() const;

        void createStatusBar();

        void createVulkanContext();

        void createRenderScene();

        void setProcedure();

        void resolveWindowResizeEnd() const;

        void checkResizeRequest() const;

        void drawFrame();

        void changeFrameIndex();

        void prepareWindow() const;

        void createMasterWindow(HMENU hMenubar);

        void createRenderWindow();

        void destroy() override;
    };
}
