//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFractal.h"

#include "Callback.h"
#include "SettingsMenu.h"
#include "../formula/Perturbator.h"

namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackFractal::REFERENCE = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &calc = scene.getSettings().calculationSettings;
        auto window = std::make_unique<SettingsWindow>(L"Reference");

        auto centerPtr = std::make_shared<std::array<std::string, 2> >();
        *centerPtr = {calc.center.real.to_string(), calc.center.imag.to_string()};

        auto zoomPtr = std::make_shared<float>(calc.logZoom);
        auto locationChanged = std::make_shared<bool>(false);


        window->registerTextInput<std::string>("Real", &(*centerPtr)[0],
                                               Unparser::STRING,
                                               Parser::STRING, [](const std::string &v) {
                                                   mpf_t t;
                                                   const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                                                   mpf_clear(t);
                                                   return valid;
                                               }, [centerPtr, locationChanged] {
                                                   *locationChanged = true;
                                               }, "Real", "Sets the real part of center.");
        window->registerTextInput<std::string>("Imag", &(*centerPtr)[1],
                                               Unparser::STRING,
                                               Parser::STRING, [](const std::string &v) {
                                                   mpf_t t;
                                                   const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                                                   mpf_clear(t);
                                                   return valid;
                                               }, [centerPtr, locationChanged] {
                                                   *locationChanged = true;
                                               }, "Imag", "Sets the imaginary part of center.");
        window->registerTextInput<float>("Log Zoom", zoomPtr.get(), Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                                         [zoomPtr, locationChanged] {
                                             *locationChanged = true;
                                         }, "Log zoom", "Sets the log scale of zoom.");
        window->registerRadioButtonInput<ReuseReferenceMethod>("Reuse Reference", &calc.reuseReferenceMethod,
                                                               Callback::NOTHING, "Reuse Reference method",
                                                               "Sets the reuse reference method.");
        window->registerTextInput<uint32_t>("Reference Compression Criteria",
                                            &calc.referenceCompressionSettings.compressCriteria,
                                            Unparser::U_LONG, Parser::U_LONG,
                                            ValidCondition::ALL_U_LONG, Callback::NOTHING,
                                            "Reference Compression Criteria",
                                            "When compressing references, sets the minimum amount of references to compress at one time.\n"
                                            "Reference compression slows down the calculation but frees up memory space.\n"
                                            "Not activate option is ZERO.");
        window->registerTextInput<uint8_t>("Reference Compression Threshold",
                                           &calc.referenceCompressionSettings.compressionThresholdPower,
                                           Unparser::U_CHAR, Parser::U_CHAR,
                                           ValidCondition::ALL_U_CHAR, Callback::NOTHING,
                                           "Reference Compression Threshold Power",
                                           "When compressing references, sets the negative exponents of ten of minimum error to be considered equal.\n"
                                           "Reference compression slows down the calculation but frees up memory space.\n"
                                           "Not activate option is ZERO.");
        window->registerBoolInput("NO Compressor normalization",
                                  &calc.referenceCompressionSettings.noCompressorNormalization, Callback::NOTHING,
                                  "NO Compressor normalization",
                                  "Do not use normalization when compressing references. "
                                  "this will accelerates table creation, But may cause table creation to fail in the specific locations!!");
        window->setWindowCloseFunction(
            [centerPtr, zoomPtr, locationChanged, &settingsMenu, &scene, &calc] {
                const int exp10 = Perturbator::logZoomToExp10(*zoomPtr);
                if (*locationChanged) {
                    calc.center = fp_complex((*centerPtr)[0], (*centerPtr)[1], exp10);
                    calc.logZoom = *zoomPtr;
                    scene.requestRecompute();
                }
                settingsMenu.setCurrentActiveSettingsWindow(nullptr);
            });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackFractal::ITERATIONS = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &calc = scene.getSettings().calculationSettings;
        auto window = std::make_unique<SettingsWindow>(L"Iterations");


        window->registerTextInput<uint64_t>("Max Iteration", &calc.maxIteration,
                                                Unparser::U_LONG_LONG,
                                                Parser::U_LONG_LONG,
                                                ValidCondition::ALL_U_LONG_LONG,
                                                Callback::NOTHING, "Set Max Iteration",
                                                "Set maximum iteration. It is disabled when Auto iteration is enabled.");
        window->registerTextInput<uint16_t>("Auto Iteration Multiplier", &calc.autoIterationMultiplier,
                                                        Unparser::U_SHORT,
                                                        Parser::U_SHORT,
                                                        ValidCondition::ALL_U_SHORT,
                                                        Callback::NOTHING, "Set Auto Iteration Multiplier",
                                                        "Set auto iteration multiplier. It is disabled when Auto iteration is disabled.");

        window->registerTextInput<float>("Bailout", &calc.bailout, Unparser::FLOAT,
                                         Parser::FLOAT, [](const float &v) { return v >= 2 && v <= 8; },
                                         Callback::NOTHING, "Set Bailout", "Sets The Bailout radius"
        );
        window->registerRadioButtonInput<DecimalizeIterationMethod>("Decimalize iteration",
                                                                    &calc.decimalizeIterationMethod,
                                                                    Callback::NOTHING, "Decimalize Iteration Method",
                                                                    "Sets the decimalization method of iterations.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackFractal::MPA = [
            ](SettingsMenu &settingsMenu, GLRenderScene &scene) {
        auto &[minSkipReference, maxMultiplierBetweenLevel, epsilonPower, mpaSelectionMethod, mpaCompressionMethod] =
                scene.getSettings().calculationSettings.mpaSettings;
        auto window = std::make_unique<SettingsWindow>(L"MP-Approximation");
        window->registerTextInput<uint16_t>("Min Skip Reference", &minSkipReference, Unparser::U_SHORT,
                                            Parser::U_SHORT, [](const unsigned short &v) { return v >= 4; },
                                            Callback::NOTHING, "Min Skip Reference",
                                            "Set minimum skipping reference iteration when creating a table.");
        window->registerTextInput<uint8_t>("Max Multiplier Between Level", &maxMultiplierBetweenLevel,
                                           Unparser::U_CHAR, Parser::U_CHAR,
                                           ValidCondition::POSITIVE_U_CHAR, Callback::NOTHING,
                                           "Set maximum multiplier between adjacent skipping levels.",
                                           "This means the maximum multiplier of two adjacent periods for the new period that inserts between them,\n"
                                           "So the multiplier between the two periods may in the worst case be the square of this."
        );
        window->registerTextInput<float>("Epsilon Power", &epsilonPower, Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::NEGATIVE_FLOAT,
                                         Callback::NOTHING,
                                         "Set Epsilon power of ten.",
                                         "Useful for glitch reduction. if this value is small,\n"
                                         "The fractal will be rendered glitch-less but slow,\n"
                                         "and is large, It will be fast, but maybe shown visible glitches."
        );
        window->registerRadioButtonInput<MPASelectionMethod>("Selection Method", &mpaSelectionMethod,
                                                             Callback::NOTHING, "Set the selection method of MPA.",
                                                             "The first target PA is always the front element."
        );

        window->registerRadioButtonInput<MPACompressionMethod>("Compression Method", &mpaCompressionMethod,
                                                               Callback::NOTHING, "Set the compression method of MPA.",
                                                               "\"Little Compression\" maybe slowing down for table creation, but allocates the memory efficiently.\n"
                                                               "\"Strongest\" works based on the Reference Compressor, so if it is disabled, it will behave the same as \"Little Compression\".\n "
                                                               "It uses acceleration when possible, and can accelerate table creation by 10x~100x of times."
        );
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };

    const std::function<bool*(GLRenderScene &)> CallbackFractal::AUTOMATIC_ITERATIONS = [
            ](GLRenderScene &scene) {
        return &scene.getSettings().calculationSettings.autoMaxIteration;
    };

    const std::function<bool*(GLRenderScene &)> CallbackFractal::ABSOLUTE_ITERATION_MODE = [
            ](GLRenderScene &scene) {
        return &scene.getSettings().calculationSettings.absoluteIterationMode;
    };
}
