//
// Created by Merutilm on 2025-05-16.
//

#include "CallbackShader.hpp"
#include "Callback.hpp"
#include "SettingsWindow.hpp"

#include "../io/KFRColorLoader.hpp"

namespace merutilm::rff2 {

    std::function<void()> CallbackShader::fnPalette(AppRenderManager &arm) {
        return [&arm] {
            auto &[colors, iterationColoring, singleIterationColoring,iterationInterval, offsetRatio, animationSpeed] =
                    arm.getSettings().shader.palette;
            auto window = std::make_unique<SettingsWindow>(L"Set Palette");
            window->registerTextInput<float>(
                    L"Iteration Interval", &iterationInterval, Unparser::FLOAT, Parser::FLOAT,
                    ValidCondition::POSITIVE_FLOAT, [&arm] { arm.getRequests().requestShader(); },
                    L"Set Iteration Interval", L"Required iterations for the palette to cycle once");
            window->registerTextInput<float>(
                    L"Offset Ratio", &offsetRatio, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Offset Ratio",
                    L"Start offset ratio of cycling palette. 0.0 ~ 1.0 value required.");
            window->registerTextInput<float>(
                    L"Animation Speed", &animationSpeed, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Animation Speed",
                    L"Color Animation Speed, The colors' offset(iterations) per second.");
            window->registerRadioButtonInput<ShdIterationColoringMethod>(
                    L"Iteration Coloring", &iterationColoring, [&arm] { arm.getRequests().requestShader(); },
                    L"Iteration Coloring", L"Iteration Coloring method");
            window->registerRadioButtonInput<ShdPalSingleIterationColoringMethod>(
                    L"Single Iteration Coloring", &singleIterationColoring, [&arm] { arm.getRequests().requestShader(); },
                    L"Single Iteration Coloring", L"Single Iteration Coloring method");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnStripe(AppRenderManager &arm) {
        return [&arm] {
            auto &[stripeType, firstInterval, secondInterval, opacity, offset, animationSpeed, iterationColoring] =
                    arm.getSettings().shader.stripe;
            auto window = std::make_unique<SettingsWindow>(L"Set Stripe");
            window->registerRadioButtonInput<ShdStripeType>(
                    L"Stripe Type", &stripeType, [&arm] { arm.getRequests().requestShader(); }, L"Set Stripe Type",
                    L"Sets the stripe type");
            window->registerTextInput<float>(
                    L"Interval 1", &firstInterval, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set interval 1",
                    L"Sets the first Stripe Interval");
            window->registerTextInput<float>(
                    L"Interval 2", &secondInterval, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set interval 2",
                    L"Sets the second Stripe Interval");
            window->registerTextInput<float>(
                    L"Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set opacity", L"Sets the opacity of stripes.");
            window->registerTextInput<float>(
                    L"Offset", &offset, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set offset ratio",
                    L"Start offset iteration of stripes.");
            window->registerTextInput<float>(
                    L"Animation Speed", &animationSpeed, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Animation Speed",
                    L"Sets the stripe animation speed.");

            window->registerRadioButtonInput<ShdIterationColoringMethod>(
                   L"Iteration Coloring", &iterationColoring, [&arm] { arm.getRequests().requestShader(); },
                   L"Iteration Coloring", L"Iteration Coloring method");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnSlope(AppRenderManager &arm) {
        return [&arm] {
            auto &[depth, reflectionRatio, opacity, zenith, azimuth] = arm.getSettings().shader.slope;
            auto window = std::make_unique<SettingsWindow>(L"Set Slope");
            window->registerTextInput<float>(
                    L"Depth", &depth, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Depth", L"Sets the depth of slope.");

            window->registerTextInput<float>(
                    L"Reflection Ratio", &reflectionRatio, Unparser::FLOAT, Parser::FLOAT,
                    ValidCondition::FLOAT_ZERO_TO_ONE, [&arm] { arm.getRequests().requestShader(); },
                    L"Set Reflection Ratio", L"Sets the reflection ratio of the slope. same as minimum brightness.");

            window->registerTextInput<float>(
                    L"Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Opacity", L"Sets the opacity of the slope.");

            window->registerTextInput<float>(
                    L"Zenith", &zenith, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_DEGREE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Zenith",
                    L"Sets the zenith of the slope. 0 ~ 360 value is required.");

            window->registerTextInput<float>(
                    L"Azimuth", &azimuth, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_DEGREE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Azimuth",
                    L"Sets the azimuth of the slope. 0 ~ 360 value is required.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));

        };
    }
    std::function<void()> CallbackShader::fnColor(AppRenderManager &arm) {
        return [&arm] {
            auto &[gamma, exposure, hue, saturation, brightness, contrast] = arm.getSettings().shader.color;
            auto window = std::make_unique<SettingsWindow>(L"Set Color");
            window->registerTextInput<float>(
                    L"Gamma", &gamma, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Gamma", L"Sets the gamma.");
            window->registerTextInput<float>(
                    L"Exposure", &exposure, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Exposure", L"Sets the exposure.");
            window->registerTextInput<float>(
                    L"Hue", &hue, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set hue", L"Sets the hue.");
            window->registerTextInput<float>(
                    L"Saturation", &saturation, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Saturation", L"Sets the saturation.");
            window->registerTextInput<float>(
                    L"Brightness", &brightness, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Brightness", L"Sets the brightness.");
            window->registerTextInput<float>(
                    L"Contrast", &contrast, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Contrast", L"Sets the contrast.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnFog(AppRenderManager &arm) {
        return [&arm] {
            auto &[radius, opacity] = arm.getSettings().shader.fog;
            auto window = std::make_unique<SettingsWindow>(L"Set Fog");
            window->registerTextInput<float>(
                    L"Radius", &radius, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Radius", L"Sets the radius of the fog.");
            window->registerTextInput<float>(
                    L"Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Opacity", L"Sets the opacity of the fog.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnBloom(AppRenderManager &arm) {
        return [&arm] {
            auto &[threshold, radius, softness, intensity] = arm.getSettings().shader.bloom;
            auto window = std::make_unique<SettingsWindow>(L"Set Bloom");
            window->registerTextInput<float>(
                    L"Threshold", &threshold, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Threshold", L"Sets the threshold of the bloom.");
            window->registerTextInput<float>(
                    L"Radius", &radius, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Radius", L"Sets the radius of the bloom.");
            window->registerTextInput<float>(
                    L"Softness", &softness, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Softness", L"Sets the softness of the bloom.");
            window->registerTextInput<float>(
                    L"Intensity", &intensity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, L"Set Intensity", L"Sets the intensity of the bloom.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnLoadKfrPalette(AppRenderManager &arm) {
        return [&arm] {
            const auto colors = KFRColorLoader::loadPaletteSettings();
            if (colors.empty()) {
                MessageBoxW(nullptr, L"No colors found", L"Error", MB_OK | MB_ICONERROR);
                return;
            }
            arm.getSettings().shader.palette.colors = colors;
            arm.getRequests().requestShader();
        };
    }


} // namespace merutilm::rff2
