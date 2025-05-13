//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackFractal.h"

#include "RFFSettingsMenu.h"

const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::REFERENCE = [
        ](RFFSettingsMenu &, RFFRenderScene &) {
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::ITERATIONS = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto &calc = scene.getSettings().calculationSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Iterations");

    window->registerTextInput<uint64_t>("Max Iteration", calc.maxIteration, RFFConstants::Unparser::U_LONG_LONG,
                                        RFFConstants::Parser::U_LONG_LONG,
                                        RFFConstants::ValidCondition::ALL_U_LONG_LONG, [&calc](const uint64_t &v) {
                                            calc.maxIteration = v;
                                        }, "Set Max Iteration",
                                        "Set maximum iteration. It is disabled when Auto iteration is enabled.");
    window->registerTextInput<float>("Bailout", calc.bailout, RFFConstants::Unparser::FLOAT,
                                     RFFConstants::Parser::FLOAT, [](const float &v) { return v >= 2; },
                                     [&calc](const float &v) {
                                         calc.bailout = v;
                                     }, "Set Bailout", "Sets The Bailout radius"
    );
    window->registerSelectionInput<DecimalizeIterationMethod>("Decimalize iteration", calc.decimalizeIterationMethod,
                                                              [&calc](const DecimalizeIterationMethod &v) {
                                                                  calc.decimalizeIterationMethod = v;
                                                              }, "Decimalize Iteration Method",
                                                              "Sets the decimalization method of iterations.");
    window->setWindowCloseFunction([&settingsMenu](RFFSettingsWindow &) {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::MPA = [
        ](RFFSettingsMenu &, RFFRenderScene &) {
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::AUTOMATIC_ITERATIONS = [
        ](RFFSettingsMenu &, RFFRenderScene &) {
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::ABSOLUTE_ITERATION_MODE = [
        ](RFFSettingsMenu &, RFFRenderScene &) {
};
