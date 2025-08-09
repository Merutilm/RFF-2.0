//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include "RenderScene.hpp"
#include "SettingsMenu.h"
#include "../vulkan/configurator/GeneralPipelineConfigurator.hpp"
#include "../vulkan/def/Engine.hpp"

namespace merutilm::rff2 {

    class Application final : public mvk::Handler {
        static constexpr uint32_t MAIN_WINDOW_ATTACHMENT_INDEX = 0;
        int statusHeight = 0;
        std::array<std::string, Constants::Status::LENGTH> statusMessages = {};
        HWND masterWindow = nullptr;
        HWND renderWindow = nullptr;
        HWND statusBar = nullptr;
        std::unique_ptr<RenderScene> scene = nullptr;
        std::unique_ptr<SettingsMenu> settingsMenu = nullptr;
        std::unique_ptr<mvk::Engine> engine = nullptr;
        std::vector<std::unique_ptr<mvk::PipelineConfigurator> > shaderPrograms = {};
        uint32_t currentFrame = 0;
        bool windowResizing = false;

    public:
        explicit Application();

        ~Application() override;

        Application(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(const Application &) = delete;

        Application &operator=(Application &&) = delete;

        mvk::Engine &getEngine() const { return *engine; }

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

        void setProcedure();

        void resolveWindowResizeEnd();

        void drawFrame();

        void changeFrameIndex();

        void prepareWindow() const;

        void createMasterWindow(HMENU hMenubar);

        void createRenderScene();

        void destroy() override;
    };
}
