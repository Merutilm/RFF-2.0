//
// Created by Merutilm on 2025-05-16.
//

#include "RFFCallback.h"
#include "RFFCallbackShader.h"

const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackShader::PALETTE = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    PaletteSettings &settings = scene.getSettings().shaderSettings.paletteSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Set Palette");
    window->registerTextInput<float>("Iteration Interval", &settings.iterationInterval, Unparser::FLOAT,
                                     Parser::FLOAT, ValidCondition::POSITIVE_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Iteration Interval", "Required iterations for the palette to cycle once");
    window->registerTextInput<float>("Offset Ratio", &settings.offsetRatio, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Offset Ratio",
                                     "Start offset ratio of cycling palette. 0.0 ~ 1.0 value required.");
    window->registerTextInput<float>("Animation Speed", &settings.animationSpeed, Unparser::FLOAT,
                                     Parser::FLOAT, ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Animation Speed",
                                     "Color Animation Speed, The colors' offset(iterations) per second.");
    window->registerRadioButtonInput<ColorSmoothingSettings>("Color Smoothing", &settings.colorSmoothing, [&scene] {
        scene.requestColor();
    }, "Color Smoothing", "Color Smoothing method");

    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackShader::STRIPE = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    StripeSettings &settings = scene.getSettings().shaderSettings.stripeSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Set Stripe");
    window->registerRadioButtonInput<StripeType>("Stripe Type", &settings.stripeType, [&scene] {
        scene.requestColor();
    }, "Set Stripe Type", "Sets the stripe type");
    window->registerTextInput<float>("Interval 1", &settings.firstInterval, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::POSITIVE_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set interval 1", "Sets the first Stripe Interval");
    window->registerTextInput<float>("Interval 2", &settings.secondInterval, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::POSITIVE_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set interval 2", "Sets the second Stripe Interval");
    window->registerTextInput<float>("Opacity", &settings.opacity, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set opacity", "Sets the opacity of stripes.");
    window->registerTextInput<float>("Offset", &settings.offset, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set offset ratio", "Start offset iteration of stripes.");
    window->registerTextInput<float>("Animation Speed", &settings.animationSpeed, Unparser::FLOAT,
                                     Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Animation Speed", "Sets the stripe animation speed.");
    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackShader::SLOPE = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    SlopeSettings &settings = scene.getSettings().shaderSettings.slopeSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Set Slope");
    window->registerTextInput<float>("Depth", &settings.depth, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Depth", "Sets the depth of slope.");

    window->registerTextInput<float>("Reflection Ratio", &settings.reflectionRatio, Unparser::FLOAT,
                                     Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Reflection Ratio",
                                     "Sets the reflection ratio of the slope. same as minimum brightness.");

    window->registerTextInput<float>("Opacity", &settings.opacity, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Opacity", "Sets the opacity of the slope.");

    window->registerTextInput<float>("Zenith", &settings.zenith, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_DEGREE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Zenith", "Sets the zenith of the slope. 0 ~ 360 value is required.");

    window->registerTextInput<float>("Azimuth", &settings.azimuth, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_DEGREE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Azimuth", "Sets the azimuth of the slope. 0 ~ 360 value is required.");

    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackShader::COLOR = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    ColorSettings &settings = scene.getSettings().shaderSettings.colorSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Set Color");
    window->registerTextInput<float>("Gamma", &settings.gamma, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Gamma", "Sets the gamma.");
    window->registerTextInput<float>("Exposure", &settings.exposure, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Exposure", "Sets the exposure.");
    window->registerTextInput<float>("Hue", &settings.hue, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set hue", "Sets the hue.");
    window->registerTextInput<float>("Saturation", &settings.saturation, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Saturation", "Sets the saturation.");
    window->registerTextInput<float>("Brightness", &settings.brightness, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Brightness", "Sets the brightness.");
    window->registerTextInput<float>("Contrast", &settings.gamma, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Contrast", "Sets the contrast.");
    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackShader::FOG = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    FogSettings &settings = scene.getSettings().shaderSettings.fogSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Set Fog");
    window->registerTextInput<float>("Radius", &settings.radius, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Radius", "Sets the radius of the fog.");
    window->registerTextInput<float>("Opacity", &settings.opacity, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Opacity", "Sets the opacity of the fog.");
    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackShader::BLOOM = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    BloomSettings &settings = scene.getSettings().shaderSettings.bloomSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Set Bloom");
    window->registerTextInput<float>("Threshold", &settings.threshold, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::FLOAT_ZERO_TO_ONE, [&scene] {
                                         scene.requestColor();
                                     }, "Set Threshold", "Sets the threshold of the bloom.");
    window->registerTextInput<float>("Radius", &settings.radius, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Radius", "Sets the radius of the bloom.");
    window->registerTextInput<float>("Softness", &settings.softness, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Softness", "Sets the softness of the bloom.");
    window->registerTextInput<float>("Intensity", &settings.intensity, Unparser::FLOAT, Parser::FLOAT,
                                     ValidCondition::ALL_FLOAT, [&scene] {
                                         scene.requestColor();
                                     }, "Set Intensity", "Sets the intensity of the bloom.");
    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
