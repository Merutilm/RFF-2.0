//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackRender.h"

#include "RFFSettingsMenu.h"
#include "RFFCallback.h"


const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackRender::SET_CLARITY = [](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto window = std::make_unique<RFFSettingsWindow>("Set clarity");
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
const std::function<bool*(RFFRenderScene&)> RFFCallbackRender::ANTIALIASING = [](RFFRenderScene& scene) {
    scene.requestColor();
    return &scene.getSettings().renderSettings.antialiasing;
};
