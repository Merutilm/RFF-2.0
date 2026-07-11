//
// Created by Merutilm on 2025-05-16.
//

#include "CallbackShader.hpp"
#include "Callback.hpp"
#include "SettingsWindow.hpp"

#include "../io/KFRColorLoader.hpp"

namespace merutilm::rff2 {

#ifdef _WIN32
    std::function<void()> CallbackShader::fnPalette(AppRenderManager &arm) {
        return [&arm] {

            auto &[colors, iterationColoring, singleIterationColoring,iterationInterval, offsetRatio, animationSpeed] =
                    arm.getSettings().shader.palette;
            auto window = std::make_unique<SettingsWindow>("Set Palette");
            window->registerTextInput<float>(
                    "Iteration Interval", &iterationInterval, Unparser::FLOAT, Parser::FLOAT,
                    ValidCondition::POSITIVE_FLOAT, [&arm] { arm.getRequests().requestShader(); },
                    "Set Iteration Interval", "Required iterations for the palette to cycle once");
            window->registerTextInput<float>(
                    "Offset Ratio", &offsetRatio, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Offset Ratio",
                    "Start offset ratio of cycling palette. 0.0 ~ 1.0 value required.");
            window->registerTextInput<float>(
                    "Animation Speed", &animationSpeed, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Animation Speed",
                    "Color Animation Speed, The colors' offset(iterations) per second.");
            window->registerRadioButtonInput<ShdIterationColoringMethod>(
                    "Iteration Coloring", &iterationColoring, [&arm] { arm.getRequests().requestShader(); },
                    "Iteration Coloring", "Iteration Coloring method");
            window->registerRadioButtonInput<ShdPalSingleIterationColoringMethod>(
                    "Single Iteration Coloring", &singleIterationColoring, [&arm] { arm.getRequests().requestShader(); },
                    "Single Iteration Coloring", "Single Iteration Coloring method");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnStripe(AppRenderManager &arm) {
        return [&arm] {
            auto &[stripeType, firstInterval, secondInterval, opacity, offset, animationSpeed, iterationColoring] =
                    arm.getSettings().shader.stripe;
            auto window = std::make_unique<SettingsWindow>("Set Stripe");
            window->registerRadioButtonInput<ShdStripeType>(
                    "Stripe Type", &stripeType, [&arm] { arm.getRequests().requestShader(); }, "Set Stripe Type",
                    "Sets the stripe type");
            window->registerTextInput<float>(
                    "Interval 1", &firstInterval, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set interval 1",
                    "Sets the first Stripe Interval");
            window->registerTextInput<float>(
                    "Interval 2", &secondInterval, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set interval 2",
                    "Sets the second Stripe Interval");
            window->registerTextInput<float>(
                    "Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set opacity", "Sets the opacity of stripes.");
            window->registerTextInput<float>(
                    "Offset", &offset, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set offset ratio",
                    "Start offset iteration of stripes.");
            window->registerTextInput<float>(
                    "Animation Speed", &animationSpeed, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Animation Speed",
                    "Sets the stripe animation speed.");

            window->registerRadioButtonInput<ShdIterationColoringMethod>(
                   "Iteration Coloring", &iterationColoring, [&arm] { arm.getRequests().requestShader(); },
                   "Iteration Coloring", "Iteration Coloring method");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnSlope(AppRenderManager &arm) {
        return [&arm] {
            auto &[depth, reflectionRatio, opacity, zenith, azimuth] = arm.getSettings().shader.slope;
            auto window = std::make_unique<SettingsWindow>("Set Slope");
            window->registerTextInput<float>(
                    "Depth", &depth, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Depth", "Sets the depth of slope.");

            window->registerTextInput<float>(
                    "Reflection Ratio", &reflectionRatio, Unparser::FLOAT, Parser::FLOAT,
                    ValidCondition::FLOAT_ZERO_TO_ONE, [&arm] { arm.getRequests().requestShader(); },
                    "Set Reflection Ratio", "Sets the reflection ratio of the slope. same as minimum brightness.");

            window->registerTextInput<float>(
                    "Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Opacity", "Sets the opacity of the slope.");

            window->registerTextInput<float>(
                    "Zenith", &zenith, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_DEGREE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Zenith",
                    "Sets the zenith of the slope. 0 ~ 360 value is required.");

            window->registerTextInput<float>(
                    "Azimuth", &azimuth, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_DEGREE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Azimuth",
                    "Sets the azimuth of the slope. 0 ~ 360 value is required.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));

        };
    }
    std::function<void()> CallbackShader::fnColor(AppRenderManager &arm) {
        return [&arm] {
            auto &[gamma, exposure, hue, saturation, brightness, contrast] = arm.getSettings().shader.color;
            auto window = std::make_unique<SettingsWindow>("Set Color");
            window->registerTextInput<float>(
                    "Gamma", &gamma, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Gamma", "Sets the gamma.");
            window->registerTextInput<float>(
                    "Exposure", &exposure, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Exposure", "Sets the exposure.");
            window->registerTextInput<float>(
                    "Hue", &hue, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set hue", "Sets the hue.");
            window->registerTextInput<float>(
                    "Saturation", &saturation, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Saturation", "Sets the saturation.");
            window->registerTextInput<float>(
                    "Brightness", &brightness, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Brightness", "Sets the brightness.");
            window->registerTextInput<float>(
                    "Contrast", &contrast, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Contrast", "Sets the contrast.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnFog(AppRenderManager &arm) {
        return [&arm] {
            auto &[radius, opacity] = arm.getSettings().shader.fog;
            auto window = std::make_unique<SettingsWindow>("Set Fog");
            window->registerTextInput<float>(
                    "Radius", &radius, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Radius", "Sets the radius of the fog.");
            window->registerTextInput<float>(
                    "Opacity", &opacity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Opacity", "Sets the opacity of the fog.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackShader::fnBloom(AppRenderManager &arm) {
        return [&arm] {
            auto &[threshold, radius, softness, intensity] = arm.getSettings().shader.bloom;
            auto window = std::make_unique<SettingsWindow>("Set Bloom");
            window->registerTextInput<float>(
                    "Threshold", &threshold, Unparser::FLOAT, Parser::FLOAT, ValidCondition::FLOAT_ZERO_TO_ONE,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Threshold", "Sets the threshold of the bloom.");
            window->registerTextInput<float>(
                    "Radius", &radius, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Radius", "Sets the radius of the bloom.");
            window->registerTextInput<float>(
                    "Softness", &softness, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Softness", "Sets the softness of the bloom.");
            window->registerTextInput<float>(
                    "Intensity", &intensity, Unparser::FLOAT, Parser::FLOAT, ValidCondition::ALL_FLOAT,
                    [&arm] { arm.getRequests().requestShader(); }, "Set Intensity", "Sets the intensity of the bloom.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
#endif
    std::function<void()> CallbackShader::fnLoadKfrPalette(RFFApplication &arm) {
        return [&arm] {
            const auto colors = KFRColorLoader::loadPaletteSettings();
            if (colors.empty()) {
                vkh::logger::log_err("No colors found");
                return;
            }
            arm.getSettings().shader.palette.colors = colors;
            arm.getRequests().requestShader();
        };
    }


} // namespace merutilm::rff2