//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackFractal.h"

#include "RFFSettingsMenu.h"
#include "../formula/Perturbator.h"

const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::REFERENCE = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto &calc = scene.getSettings().calculationSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Reference");

    auto centerPtr = std::make_shared<std::array<std::string, 2> >();
    *centerPtr = {calc.center.real.toString(), calc.center.imag.toString()};

    auto zoomPtr = std::make_shared<float>(calc.logZoom);
    auto locationChanged = std::make_shared<bool>(false);


    window->registerTextInput<std::string>("Real", &(*centerPtr)[0],
                                           RFF::Unparser::STRING,
                                           RFF::Parser::STRING, [](const std::string &v) {
                                               mpf_t t;
                                               const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                                               mpf_clear(t);
                                               return valid;
                                           }, [centerPtr, locationChanged] {
                                               *locationChanged = true;
                                           }, "Real", "Sets the real part of center.");
    window->registerTextInput<std::string>("Imag", &(*centerPtr)[1],
                                           RFF::Unparser::STRING,
                                           RFF::Parser::STRING, [](const std::string &v) {
                                               mpf_t t;
                                               const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                                               mpf_clear(t);
                                               return valid;
                                           }, [centerPtr, locationChanged] {
                                               *locationChanged = true;
                                           }, "Imag", "Sets the imaginary part of center.");
    window->registerTextInput<float>("Log Zoom", &calc.logZoom, RFF::Unparser::FLOAT,
                                     RFF::Parser::FLOAT, RFF::ValidCondition::POSITIVE_FLOAT,
                                     [zoomPtr, locationChanged] {
                                         *locationChanged = true;
                                     }, "Log zoom", "Sets the log scale of zoom.");
    window->registerRadioButtonInput<ReuseReferenceMethod>("Reuse Reference", &calc.reuseReferenceMethod,
                                                           [&calc] {
                                                           }, "Reuse Reference method",
                                                           "Sets the reuse reference method.");
    window->registerTextInput<uint32_t>("Reference Compression Criteria",
                                        &calc.referenceCompressionSettings.compressCriteria,
                                        RFF::Unparser::U_LONG, RFF::Parser::U_LONG,
                                        RFF::ValidCondition::ALL_U_LONG, [&calc] {
                                        }, "Reference Compression Criteria",
                                        "When compressing references, sets the minimum amount of references to compress at one time.\n"
                                        "Reference compression slows down the calculation but frees up memory space.\n"
                                        "Not activate option is ZERO.");
    window->registerTextInput<uint8_t>("Reference Compression Threshold",
                                       &calc.referenceCompressionSettings.compressionThresholdPower,
                                       RFF::Unparser::U_CHAR, RFF::Parser::U_CHAR,
                                       RFF::ValidCondition::ALL_U_CHAR, [&calc] {
                                       }, "Reference Compression Threshold Power",
                                       "When compressing references, sets the negative exponents of ten of minimum error to be considered equal.\n"
                                       "Reference compression slows down the calculation but frees up memory space.\n"
                                       "Not activate option is ZERO.");

    window->setWindowCloseFunction(
        [centerPtr, zoomPtr, locationChanged, &settingsMenu, &scene, &calc] {
            const int exp10 = Perturbator::logZoomToExp10(*zoomPtr);
            if (*locationChanged) {
                calc.center = Center((*centerPtr)[0], (*centerPtr)[1], exp10);
                calc.logZoom = *zoomPtr;
                scene.requestRecompute();
            }
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::ITERATIONS = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto &calc = scene.getSettings().calculationSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Iterations");

    const HWND wnd = window->registerTextInput<uint64_t>("Max Iteration", &calc.maxIteration,
                                                         RFF::Unparser::U_LONG_LONG,
                                                         RFF::Parser::U_LONG_LONG,
                                                         RFF::ValidCondition::ALL_U_LONG_LONG,
                                                         RFF::Callback::NOTHING, "Set Max Iteration",
                                                         "Set maximum iteration. It is disabled when Auto iteration is enabled.");
    SendMessage(wnd, EM_SETREADONLY, calc.autoMaxIteration, 0);
    EnableWindow(wnd, !calc.autoMaxIteration);


    window->registerTextInput<float>("Bailout", &calc.bailout, RFF::Unparser::FLOAT,
                                     RFF::Parser::FLOAT, [](const float &v) { return v >= 2 && v <= 8; },
                                     RFF::Callback::NOTHING, "Set Bailout", "Sets The Bailout radius"
    );
    window->registerSelectionInput<DecimalizeIterationMethod>("Decimalize iteration", &calc.decimalizeIterationMethod,
                                                              RFF::Callback::NOTHING, "Decimalize Iteration Method",
                                                              "Sets the decimalization method of iterations.");
    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackFractal::MPA = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto &mpa = scene.getSettings().calculationSettings.mpaSettings;
    auto window = std::make_unique<RFFSettingsWindow>("MPA");
    window->registerTextInput<uint16_t>("Min Skip Reference", &mpa.minSkipReference, RFF::Unparser::U_SHORT,
                                        RFF::Parser::U_SHORT, RFF::ValidCondition::POSITIVE_U_SHORT,
                                        RFF::Callback::NOTHING, "Min Skip Reference",
                                        "Set minimum skipping reference iteration when creating a table.");
    window->registerTextInput<uint8_t>("Max Multiplier Between Level", &mpa.maxMultiplierBetweenLevel,
                                       RFF::Unparser::U_CHAR, RFF::Parser::U_CHAR,
                                       RFF::ValidCondition::POSITIVE_U_CHAR, RFF::Callback::NOTHING,
                                       "Set maximum multiplier between adjacent skipping levels.",
                                       "This means the maximum multiplier of two adjacent periods for the new period that inserts between them,\n"
                                       "So the multiplier between the two periods may in the worst case be the square of this."
    );
    window->registerTextInput<float>("Epsilon Power", &mpa.epsilonPower, RFF::Unparser::FLOAT,
                                     RFF::Parser::FLOAT, RFF::ValidCondition::NEGATIVE_FLOAT,
                                     RFF::Callback::NOTHING,
                                     "Set Epsilon power of ten.",
                                     "Useful for glitch reduction. if this value is small,\n"
                                     "The fractal will be rendered glitch-less but slow,\n"
                                     "and is large, It will be fast, but maybe shown visible glitches."
    );
    window->registerSelectionInput<MPASelectionMethod>("Selection Method", &mpa.mpaSelectionMethod,
                                                       RFF::Callback::NOTHING, "Set the selection method of MPA.",
                                                       "The first target PA is always the front element."
    );

    window->registerSelectionInput<MPACompressionMethod>("Compression Method", &mpa.mpaCompressionMethod,
                                                         RFF::Callback::NOTHING, "Set the compression method of MPA.",
                                                         "\"Little Compression\" maybe slowing down for table creation, but allocates the memory efficiently.\n"
                                                         "\"Strongest\" works based on the Reference Compressor, so if it is disabled, it will behave the same as \"Little Compression\".\n "
                                                         "It uses acceleration when possible, and can accelerate table creation by 10x~100x of times."
    );
    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};

const std::function<bool*(RFFRenderScene &)> RFFCallbackFractal::AUTOMATIC_ITERATIONS = [
        ](RFFRenderScene &scene) {
    return &scene.getSettings().calculationSettings.autoMaxIteration;
};

const std::function<bool*(RFFRenderScene &)> RFFCallbackFractal::ABSOLUTE_ITERATION_MODE = [
        ](RFFRenderScene &scene) {
    return &scene.getSettings().calculationSettings.absoluteIterationMode;
};
