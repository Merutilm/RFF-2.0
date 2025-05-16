//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackRender.h"

#include "RFFSettingsMenu.h"


const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackRender::SET_CLARITY = [](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto window = std::make_unique<RFFSettingsWindow>("Set clarity");
    auto &render = scene.getSettings().renderSettings;
    window->registerTextInput<float>("Clarity", &render.clarityMultiplier, RFF::Unparser::FLOAT, RFF::Parser::FLOAT, [](const float &v) {
        return v > 0.05 && v <= 1;
    }, [&render, &scene] {
                                         scene.requestClarity();
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
