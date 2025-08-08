//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackRender.h"

#include "SettingsMenu.h"
#include "Callback.h"


namespace merutilm::rff2 {
    const std::function<void(SettingsMenu&, GLRenderScene&)> CallbackRender::SET_CLARITY = [](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto window = std::make_unique<SettingsWindow>(L"Set clarity");
        auto &[clarityMultiplier, antialiasing] = scene.getSettings().renderSettings;
        window->registerTextInput<float>("Clarity", &clarityMultiplier, Unparser::FLOAT, Parser::FLOAT, [](const float &v) {
            return v > 0.05 && v <= 4;
        }, [&scene] {
                                             scene.requestResize();
                                             scene.requestRecompute();
        }, "Clarity Multiplier", "Sets the clarity.");

        window->setWindowCloseFunction([&settingsMenu]{
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<bool*(GLRenderScene&)> CallbackRender::ANTIALIASING = [](GLRenderScene& scene) {
        scene.requestColor();
        return &scene.getSettings().renderSettings.antialiasing;
    };
}