//
// Created by Merutilm on 2025-05-16.
//

#include "Callback.hpp"
#include "CallbackShader.hpp"

#include "../io/KFRColorLoader.hpp"

namespace merutilm::rff2 {

    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::PALETTE = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[colors, colorSmoothing, iterationInterval, offsetRatio, animationSpeed] = scene.getAttribute().shader.palette;
        auto window = std::make_unique<SettingsWindow>(L"Set Palette");
        window->registerTextInput<float>(L"Iteration Interval", &iterationInterval, Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         },L"Set Iteration Interval", L"Required iterations for the palette to cycle once");
        window->registerTextInput<float>(L"Offset Ratio", &offsetRatio, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Offset Ratio",
                                         L"Start offset ratio of cycling palette. 0.0 ~ 1.0 value required.");
        window->registerTextInput<float>(L"Animation Speed", &animationSpeed, Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Animation Speed",
                                         L"Color Animation Speed, The colors' offset(iterations) per second.");
        window->registerRadioButtonInput<ShdPalColorSmoothingMethod>(L"Color Smoothing", &colorSmoothing, [&scene] {
            scene.getRequests().requestShader();
        }, L"Color Smoothing", L"Color Smoothing method");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::STRIPE = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[stripeType, firstInterval, secondInterval, opacity, offset, animationSpeed] = scene.getAttribute().shader.stripe;
        auto window = std::make_unique<SettingsWindow>(L"Set Stripe");
        window->registerRadioButtonInput<ShdStripeType>(L"Stripe Type", &stripeType, [&scene] {
            scene.getRequests().requestShader();
        }, L"Set Stripe Type", L"Sets the stripe type");
        window->registerTextInput<float>(L"Interval 1", &firstInterval, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set interval 1", L"Sets the first Stripe Interval");
        window->registerTextInput<float>(L"Interval 2", &secondInterval, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set interval 2", L"Sets the second Stripe Interval");
        window->registerTextInput<float>(L"Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set opacity", L"Sets the opacity of stripes.");
        window->registerTextInput<float>(L"Offset", &offset, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set offset ratio", L"Start offset iteration of stripes.");
        window->registerTextInput<float>(L"Animation Speed", &animationSpeed, Unparser::FLOAT,
                                         Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Animation Speed", L"Sets the stripe animation speed.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::SLOPE = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[depth, reflectionRatio, opacity, zenith, azimuth] = scene.getAttribute().shader.slope;
        auto window = std::make_unique<SettingsWindow>(L"Set Slope");
        window->registerTextInput<float>(L"Depth", &depth, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Depth", L"Sets the depth of slope.");

        window->registerTextInput<float>(L"Reflection Ratio", &reflectionRatio, Unparser::FLOAT,
                                         Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Reflection Ratio",
                                         L"Sets the reflection ratio of the slope. same as minimum brightness.");

        window->registerTextInput<float>(L"Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Opacity", L"Sets the opacity of the slope.");

        window->registerTextInput<float>(L"Zenith", &zenith, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_DEGREE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Zenith", L"Sets the zenith of the slope. 0 ~ 360 value is required.");

        window->registerTextInput<float>(L"Azimuth", &azimuth, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_DEGREE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Azimuth", L"Sets the azimuth of the slope. 0 ~ 360 value is required.");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::COLOR = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[gamma, exposure, hue, saturation, brightness, contrast] = scene.getAttribute().shader.color;
        auto window = std::make_unique<SettingsWindow>(L"Set Color");
        window->registerTextInput<float>(L"Gamma", &gamma, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Gamma", L"Sets the gamma.");
        window->registerTextInput<float>(L"Exposure", &exposure, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Exposure", L"Sets the exposure.");
        window->registerTextInput<float>(L"Hue", &hue, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set hue", L"Sets the hue.");
        window->registerTextInput<float>(L"Saturation", &saturation, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Saturation", L"Sets the saturation.");
        window->registerTextInput<float>(L"Brightness", &brightness, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Brightness", L"Sets the brightness.");
        window->registerTextInput<float>(L"Contrast", &contrast, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Contrast", L"Sets the contrast.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::FOG = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[radius, opacity] = scene.getAttribute().shader.fog;
        auto window = std::make_unique<SettingsWindow>(L"Set Fog");
        window->registerTextInput<float>(L"Radius", &radius, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Radius", L"Sets the radius of the fog.");
        window->registerTextInput<float>(L"Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Opacity", L"Sets the opacity of the fog.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::BLOOM = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[threshold, radius, softness, intensity] = scene.getAttribute().shader.bloom;
        auto window = std::make_unique<SettingsWindow>(L"Set Bloom");
        window->registerTextInput<float>(L"Threshold", &threshold, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Threshold", L"Sets the threshold of the bloom.");
        window->registerTextInput<float>(L"Radius", &radius, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Radius", L"Sets the radius of the bloom.");
        window->registerTextInput<float>(L"Softness", &softness, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Softness", L"Sets the softness of the bloom.");
        window->registerTextInput<float>(L"Intensity", &intensity, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::ALL_FLOAT, [&scene] {
                                             scene.getRequests().requestShader();
                                         }, L"Set Intensity", L"Sets the intensity of the bloom.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackShader::LOAD_KFR_PALETTE = [
            ](SettingsMenu &, RenderScene &scene) {
        const auto colors = KFRColorLoader::loadPaletteSettings();
        if (colors.empty()) {
            MessageBoxW(nullptr, L"No colors found", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        scene.getAttribute().shader.palette.colors = colors;
        scene.getRequests().requestShader();
    };
}
