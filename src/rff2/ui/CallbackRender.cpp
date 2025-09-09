//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackRender.hpp"

#include "SettingsMenu.hpp"
#include "Callback.hpp"


namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackRender::SET_CLARITY = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto window = std::make_unique<SettingsWindow>(L"Set Render Properties");
        auto &[clarityMultiplier, fps, linearInterpolation, threads] = scene.getAttribute().render;
        window->registerTextInput<float>(L"Clarity", &clarityMultiplier, Unparser::FLOAT, Parser::FLOAT,
                                         [](const float &v) {
                                             return v > 0.05 && v <= 4;
                                         }, [&scene] {
                                             scene.getRequests().requestResize();
                                             scene.getRequests().requestRecompute();
                                         }, L"Clarity Multiplier", L"Sets the clarity multiplier.");
        window->registerTextInput<float>(L"Framerate", &fps, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.wndRequestFPS();
                                         }, L"Framerate per second",
                                         L"Sets the Framerate.");
        window->registerTextInput<uint32_t>(L"Threads", &threads, Unparser::U_LONG, Parser::U_LONG,
                                         ValidCondition::ALL_U_LONG, Callback::NOTHING, L"Threads",
                                         L"Sets the number of threads when calculating.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<bool*(RenderScene &, bool)> CallbackRender::LINEAR_INTERPOLATION = [
            ](RenderScene &scene, const bool executeMode) {
        if (executeMode) {
            scene.getRequests().requestShader();
        }
        return &scene.getAttribute().render.linearInterpolation;
    };
}
