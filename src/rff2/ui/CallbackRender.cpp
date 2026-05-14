//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackRender.hpp"

#include "Callback.hpp"
#include "SettingsMenuGenerator.hpp"


namespace merutilm::rff2 {

    std::function<void()> CallbackRender::fnSetRenderProperties(AppRenderManager &arm) {
        return [&arm] {
            auto window = std::make_unique<SettingsWindow>(L"Set Render Properties");
            auto &[clarityMultiplier, fps, linearInterpolation, threads] = arm.getSettings().render;
            window->registerTextInput<float>(
                    L"Clarity", &clarityMultiplier, Unparser::FLOAT, Parser::FLOAT,
                    [](const float &v) { return v > 0.05 && v <= 4; },
                    [&arm] {
                        arm.getRequests().requestResize();
                        arm.getRequests().requestRecompute();
                    },
                    L"Clarity Multiplier", L"Sets the clarity multiplier.");
            window->registerTextInput<float>(
                    L"Framerate", &fps, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [&arm, &fps] {
                        arm.getWindowContext().getWindow()->initializerSettings.framerate = fps;
                    }, L"Framerate per second", L"Sets the Framerate.");
            window->registerTextInput<uint32_t>(L"Threads", &threads, Unparser::UINT32, Parser::UINT32,
                                                ValidCondition::ALL_UINT32, Callback::NOTHING, L"Threads",
                                                L"Sets the number of threads while rendering an image.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<bool()> CallbackRender::fnGetterLinearInterpolation(AppRenderManager &arm) {
        return [&arm] {
            return arm.getSettings().render.linearInterpolation;
        };
    }
    std::function<void(bool)> CallbackRender::fnLinearInterpolation(AppRenderManager &arm) {
        return [&arm](const bool b) {
            arm.getSettings().render.linearInterpolation = b;
            arm.getRequests().requestShader();
        };
    }

    
} // namespace merutilm::rff2
