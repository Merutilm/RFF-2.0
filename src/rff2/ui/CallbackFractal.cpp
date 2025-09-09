//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFractal.hpp"

#include "Callback.hpp"
#include "SettingsMenu.hpp"
#include "../formula/Perturbator.h"

namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackFractal::REFERENCE = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &calc = scene.getAttribute().fractal;
        auto window = std::make_unique<SettingsWindow>(L"Reference");

        auto centerPtr = std::make_shared<std::array<std::string, 2> >();
        *centerPtr = {calc.center.real.to_string(), calc.center.imag.to_string()};

        auto zoomPtr = std::make_shared<float>(calc.logZoom);
        auto locationChanged = std::make_shared<bool>(false);


        window->registerTextInput<std::string>(L"Real", &(*centerPtr)[0],
                                               Unparser::STRING,
                                               Parser::STRING, [](const std::string &v) {
                                                   mpf_t t;
                                                   const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                                                   mpf_clear(t);
                                                   return valid;
                                               }, [centerPtr, locationChanged] {
                                                   *locationChanged = true;
                                               }, L"Real", L"Sets the real part of center.");
        window->registerTextInput<std::string>(L"Imag", &(*centerPtr)[1],
                                               Unparser::STRING,
                                               Parser::STRING, [](const std::string &v) {
                                                   mpf_t t;
                                                   const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                                                   mpf_clear(t);
                                                   return valid;
                                               }, [centerPtr, locationChanged] {
                                                   *locationChanged = true;
                                               }, L"Imag", L"Sets the imaginary part of center.");
        window->registerTextInput<float>(L"Log Zoom", zoomPtr.get(), Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                                         [zoomPtr, locationChanged] {
                                             *locationChanged = true;
                                         }, L"Log zoom", L"Sets the log scale of zoom.");
        window->registerRadioButtonInput<FrtReuseReferenceMethod>(L"Reuse Reference", &calc.reuseReferenceMethod,
                                                               Callback::NOTHING, L"Reuse Reference method",
                                                               L"Sets the reuse reference method.");
        window->registerTextInput<uint32_t>(L"Reference Compression Criteria",
                                            &calc.referenceCompAttribute.compressCriteria,
                                            Unparser::U_LONG, Parser::U_LONG,
                                            ValidCondition::ALL_U_LONG, Callback::NOTHING,
                                            L"Reference Compression Criteria",
                                            L"When compressing references, sets the minimum amount of references to compress at one time.\n"
                                            L"Reference compression slows down the calculation but frees up memory space.\n"
                                            L"Not activate option is ZERO.");
        window->registerTextInput<uint8_t>(L"Reference Compression Threshold",
                                           &calc.referenceCompAttribute.compressionThresholdPower,
                                           Unparser::U_CHAR, Parser::U_CHAR,
                                           ValidCondition::ALL_U_CHAR, Callback::NOTHING,
                                           L"Reference Compression Threshold Power",
                                           L"When compressing references, sets the negative exponents of ten of minimum error to be considered equal.\n"
                                           L"Reference compression slows down the calculation but frees up memory space.\n"
                                           L"Not activate option is ZERO.");
        window->registerCheckboxInput(L"NO Compressor normalization",
                                  &calc.referenceCompAttribute.noCompressorNormalization, Callback::NOTHING,
                                  L"NO Compressor normalization",
                                  L"Do not use normalization when compressing references. L"
                                  L"this will accelerates table creation, But may cause table creation to fail in the specific locations!!");
        window->setWindowCloseFunction(
            [centerPtr, zoomPtr, locationChanged, &settingsMenu, &scene, &calc] {
                const int exp10 = Perturbator::logZoomToExp10(*zoomPtr);
                if (*locationChanged) {
                    calc.center = fp_complex((*centerPtr)[0], (*centerPtr)[1], exp10);
                    calc.logZoom = *zoomPtr;
                    scene.getRequests().requestRecompute();
                }
                settingsMenu.setCurrentActiveSettingsWindow(nullptr);
            });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackFractal::ITERATIONS = [
            ](SettingsMenu &settingsMenu, RenderScene  &scene) {
        auto &calc = scene.getAttribute().fractal;
        auto window = std::make_unique<SettingsWindow>(L"Iterations");


        window->registerTextInput<uint64_t>(L"Max Iteration", &calc.maxIteration,
                                                Unparser::U_LONG_LONG,
                                                Parser::U_LONG_LONG,
                                                ValidCondition::ALL_U_LONG_LONG,
                                                Callback::NOTHING, L"Set Max Iteration",
                                                L"Set maximum iteration. It is disabled when Auto iteration is enabled.");
        window->registerTextInput<uint16_t>(L"Auto Iteration Multiplier", &calc.autoIterationMultiplier,
                                                        Unparser::U_SHORT,
                                                        Parser::U_SHORT,
                                                        ValidCondition::ALL_U_SHORT,
                                                        Callback::NOTHING, L"Set Auto Iteration Multiplier",
                                                        L"Set auto iteration multiplier. It is disabled when Auto iteration is disabled.");

        window->registerTextInput<float>(L"Bailout", &calc.bailout, Unparser::FLOAT,
                                         Parser::FLOAT, [](const float &v) { return v >= 2 && v <= 8; },
                                         Callback::NOTHING, L"Set Bailout", L"Sets The Bailout radius"
        );
        window->registerRadioButtonInput<FrtDecimalizeIterationMethod>(L"Decimalize iteration",
                                                                    &calc.decimalizeIterationMethod,
                                                                    Callback::NOTHING, L"Decimalize Iteration Method",
                                                                    L"Sets the decimalization method of iterations.");
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackFractal::MPA = [
            ](SettingsMenu &settingsMenu, RenderScene  &scene) {
        auto &[minSkipReference, maxMultiplierBetweenLevel, epsilonPower, mpaSelectionMethod, mpaCompressionMethod] =
                scene.getAttribute().fractal.mpaAttribute;
        auto window = std::make_unique<SettingsWindow>(L"MP-Approximation");
        window->registerTextInput<uint16_t>(L"Min Skip Reference", &minSkipReference, Unparser::U_SHORT,
                                            Parser::U_SHORT, [](const unsigned short &v) { return v >= 4; },
                                            Callback::NOTHING, L"Min Skip Reference",
                                            L"Set minimum skipping reference iteration when creating a table.");
        window->registerTextInput<uint8_t>(L"Max Multiplier Between Level", &maxMultiplierBetweenLevel,
                                           Unparser::U_CHAR, Parser::U_CHAR,
                                           ValidCondition::POSITIVE_U_CHAR, Callback::NOTHING,
                                           L"Set maximum multiplier between adjacent skipping levels.",
                                           L"This means the maximum multiplier of two adjacent periods for the new period that inserts between them,\n"
                                           L"So the multiplier between the two periods may in the worst case be the square of this."
        );
        window->registerTextInput<float>(L"Epsilon Power", &epsilonPower, Unparser::FLOAT,
                                         Parser::FLOAT, ValidCondition::NEGATIVE_FLOAT,
                                         Callback::NOTHING,
                                         L"Set Epsilon power of ten.",
                                         L"Useful for glitch reduction. if this value is small,\n"
                                         L"The fractal will be rendered glitch-less but slow,\n"
                                         L"and is large, It will be fast, but maybe shown visible glitches."
        );
        window->registerRadioButtonInput<FrtMPASelectionMethod>(L"Selection Method", &mpaSelectionMethod,
                                                             Callback::NOTHING, L"Set the selection method of MPA.",
                                                             L"The first target PA is always the front element."
        );

        window->registerRadioButtonInput<FrtMPACompressionMethod>(L"Compression Method", &mpaCompressionMethod,
                                                               Callback::NOTHING, L"Set the compression method of MPA.",
                                                               L"\"Little Compression\" maybe slowing down for table creation, but allocates the memory efficiently.\n"
                                                               L"\"Strongest\" works based on the Reference Compressor, so if it is disabled, it will behave the same as \"Little Compression\".\n L"
                                                               L"It uses acceleration when possible, and can accelerate table creation by 10x~100x of times."
        );
        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };

    const std::function<bool*(RenderScene &, bool)> CallbackFractal::AUTOMATIC_ITERATIONS = [
            ](RenderScene  &scene, bool) {
        return &scene.getAttribute().fractal.autoMaxIteration;
    };

    const std::function<bool*(RenderScene &, bool)> CallbackFractal::ABSOLUTE_ITERATION_MODE = [
            ](RenderScene  &scene, bool) {
        return &scene.getAttribute().fractal.absoluteIterationMode;
    };
}
