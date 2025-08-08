//
// Created by Merutilm on 2025-05-16.
//

#include "Callback.h"
#include "CallbackShader.h"

#include "../io/KFRColorLoader.hpp"

namespace merutilm::rff2 {

    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::PALETTE = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[colors, colorSmoothing, iterationInterval, offsetRatio, animationSpeed] = scene.getSettings().shaderSettings.paletteSettings;
        auto window = std::make_unique<SettingsWindow>(L"Set Palette");
        window->registerTextInput<float>("Iteration Interval", &iterationInterval, Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Iteration Interval", "Required iterations for the palette to cycle once");
        window->registerTextInput<float>("Offset Ratio", &offsetRatio, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Offset Ratio",
                                         "Start offset ratio of cycling palette. 0.0 ~ 1.0 value required.");
        window->registerTextInput<float>("Animation Speed", &animationSpeed, Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Animation Speed",
                                         "Color Animation Speed, The colors' offset(iterations) per second.");
        window->registerRadioButtonInput<ColorSmoothingSettings>("Color Smoothing", &colorSmoothing, [&scene] {
            scene.requestColor();
        }, "Color Smoothing", "Color Smoothing method");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::STRIPE = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[stripeType, firstInterval, secondInterval, opacity, offset, animationSpeed] = scene.getSettings().shaderSettings.stripeSettings;
        auto window = std::make_unique<SettingsWindow>(L"Set Stripe");
        window->registerRadioButtonInput<StripeType>("Stripe Type", &stripeType, [&scene] {
            scene.requestColor();
        }, "Set Stripe Type", "Sets the stripe type");
        window->registerTextInput<float>("Interval 1", &firstInterval, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set interval 1", "Sets the first Stripe Interval");
        window->registerTextInput<float>("Interval 2", &secondInterval, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set interval 2", "Sets the second Stripe Interval");
        window->registerTextInput<float>("Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set opacity", "Sets the opacity of stripes.");
        window->registerTextInput<float>("Offset", &offset, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set offset ratio", "Start offset iteration of stripes.");
        window->registerTextInput<float>("Animation Speed", &animationSpeed, Unparser::FLOAT,
                                         Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Animation Speed", "Sets the stripe animation speed.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::SLOPE = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[depth, reflectionRatio, opacity, zenith, azimuth] = scene.getSettings().shaderSettings.slopeSettings;
        auto window = std::make_unique<SettingsWindow>(L"Set Slope");
        window->registerTextInput<float>("Depth", &depth, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Depth", "Sets the depth of slope.");

        window->registerTextInput<float>("Reflection Ratio", &reflectionRatio, Unparser::FLOAT,
                                         Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Reflection Ratio",
                                         "Sets the reflection ratio of the slope. same as minimum brightness.");

        window->registerTextInput<float>("Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Opacity", "Sets the opacity of the slope.");

        window->registerTextInput<float>("Zenith", &zenith, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_DEGREE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Zenith", "Sets the zenith of the slope. 0 ~ 360 value is required.");

        window->registerTextInput<float>("Azimuth", &azimuth, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_DEGREE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Azimuth", "Sets the azimuth of the slope. 0 ~ 360 value is required.");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::COLOR = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[gamma, exposure, hue, saturation, brightness, contrast] = scene.getSettings().shaderSettings.colorSettings;
        auto window = std::make_unique<SettingsWindow>(L"Set Color");
        window->registerTextInput<float>("Gamma", &gamma, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Gamma", "Sets the gamma.");
        window->registerTextInput<float>("Exposure", &exposure, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Exposure", "Sets the exposure.");
        window->registerTextInput<float>("Hue", &hue, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set hue", "Sets the hue.");
        window->registerTextInput<float>("Saturation", &saturation, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Saturation", "Sets the saturation.");
        window->registerTextInput<float>("Brightness", &brightness, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Brightness", "Sets the brightness.");
        window->registerTextInput<float>("Contrast", &gamma, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Contrast", "Sets the contrast.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::FOG = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[radius, opacity] = scene.getSettings().shaderSettings.fogSettings;
        auto window = std::make_unique<SettingsWindow>(L"Set Fog");
        window->registerTextInput<float>("Radius", &radius, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Radius", "Sets the radius of the fog.");
        window->registerTextInput<float>("Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Opacity", "Sets the opacity of the fog.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::BLOOM = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[threshold, radius, softness, intensity] = scene.getSettings().shaderSettings.bloomSettings;
        auto window = std::make_unique<SettingsWindow>(L"Set Bloom");
        window->registerTextInput<float>("Threshold", &threshold, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.requestColor();
                                         }, "Set Threshold", "Sets the threshold of the bloom.");
        window->registerTextInput<float>("Radius", &radius, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Radius", "Sets the radius of the bloom.");
        window->registerTextInput<float>("Softness", &softness, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Softness", "Sets the softness of the bloom.");
        window->registerTextInput<float>("Intensity", &intensity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.requestColor();
                                         }, "Set Intensity", "Sets the intensity of the bloom.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackShader::LOAD_KFR_PALETTE = [
            ](SettingsMenu &, GLRenderScene &scene) {
        const auto colors = KFRColorLoader::loadPaletteSettings();
        if (colors.empty()) {
            MessageBox(nullptr, "No colors found", "Error", MB_OK | MB_ICONERROR);
            return;
        }
        scene.getSettings().shaderSettings.paletteSettings.colors = colors;
        scene.requestColor();
    };
}
