//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include "AppRenderManager.hpp"
#include "SettingsMenuGenerator.hpp"
#include "vulkan_helper/Application.hpp"
#include "vulkan_helper/engine/window/win/NativeWindow.hpp"

namespace merutilm::rff2 {

    class AppLauncher final : public vkh::Application {
        int statusHeight = 0;
        std::array<std::wstring, Constants::Status::LENGTH> statusMessages = {};
        vkh::WindowContext * wc = nullptr;
        std::unique_ptr<AppRenderManager> arm = nullptr;

        HWND statusBar = nullptr;
        bool windowResizing = false;

    public:
        explicit AppLauncher();

        ~AppLauncher() override;

        AppLauncher(const AppLauncher &) = delete;

        AppLauncher(AppLauncher &&) = delete;

        AppLauncher &operator=(const AppLauncher &) = delete;

        AppLauncher &operator=(AppLauncher &&) = delete;

        vkh::Engine &getEngine() { return *engine; }

        void start() const;

    private:
        void init() override;

        void initWindow();

        void initMenu() const;

        void refreshStatusBar() const;

        void createStatusBar();

        void createRenderManager();

        void setProcedure() const;

        [[nodiscard]] vkh::NativeWindow &getNativeWindow() const {
            return *dynamic_cast<vkh::NativeWindow *>(wc->getWindow());
        }

        void cleanup() override;
    };
}
