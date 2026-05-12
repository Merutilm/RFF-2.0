//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFractal.hpp"

#include "../formula/Perturbator.h"
#include "Callback.hpp"
#include "SettingsMenuGenerator.hpp"

namespace merutilm::rff2 {

    std::function<void()> CallbackFractal::fnReference(AppRenderManager &arm) {
        return [&arm] {
            auto &calc = arm.getSettings().fractal;
            auto window = std::make_unique<SettingsWindow>(L"Reference");

            auto centerPtr = std::make_shared<std::array<std::string, 2>>();
            *centerPtr = {calc.center.real.to_string(), calc.center.imag.to_string()};

            auto zoomPtr = std::make_shared<float>(calc.logZoom);
            auto locationChanged = std::make_shared<bool>(false);


            window->registerTextInput<std::string>(
                    L"Real", &(*centerPtr)[0], Unparser::STRING, Parser::STRING,
                    [](const std::string &v) {
                        mpf_t t;
                        const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                        mpf_clear(t);
                        return valid;
                    },
                    [centerPtr, locationChanged] { *locationChanged = true; }, L"Real",
                    L"Sets the real part of center.");
            window->registerTextInput<std::string>(
                    L"Imag", &(*centerPtr)[1], Unparser::STRING, Parser::STRING,
                    [](const std::string &v) {
                        mpf_t t;
                        const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                        mpf_clear(t);
                        return valid;
                    },
                    [centerPtr, locationChanged] { *locationChanged = true; }, L"Imag",
                    L"Sets the imaginary part of center.");
            window->registerTextInput<float>(
                    L"Log Zoom", zoomPtr.get(), Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [zoomPtr, locationChanged] { *locationChanged = true; }, L"Log zoom",
                    L"Sets the log scale of zoom.");
            window->registerRadioButtonInput<FrtReuseReferenceMethod>(L"Reuse Reference", &calc.reuseReferenceMethod,
                                                                      Callback::NOTHING, L"Reuse Reference method",
                                                                      L"Sets the reuse reference method.");
            window->registerTextInput<uint32_t>(
                    L"Reference Compression Criteria", &calc.referenceCompSettings.compressCriteria, Unparser::UINT32,
                    Parser::UINT32, ValidCondition::ALL_UINT32, Callback::NOTHING, L"Reference Compression Criteria",
                    L"When compressing references, sets the minimum amount of references to compress at one time.\n"
                    L"Reference compression slows down the calculation but frees up memory space.\n"
                    L"set 0 to disable.");
            window->registerTextInput<uint8_t>(
                    L"Reference Compression Threshold", &calc.referenceCompSettings.compressionThresholdPower,
                    Unparser::UINT8, Parser::UINT8, ValidCondition::ALL_UINT8, Callback::NOTHING,
                    L"Reference Compression Threshold Power",
                    L"When compressing references, sets the negative exponents of ten of minimum error to be "
                    L"considered "
                    L"equal.\n"
                    L"Reference compression slows down the calculation but frees up memory space.\n"
                    L"set 0 to disable.");
            window->registerCheckboxInput(
                    L"NO Compressor normalization", &calc.referenceCompSettings.noCompressorNormalization,
                    Callback::NOTHING, L"NO Compressor normalization",
                    L"No normalization during reference compression.\n"
                    L"this will accelerates table creation, But may cause table creation to fail in\n"
                    L"the specific locations!!");
            window->registerTextInput<uint32_t>(
                    L"Reference Synchronization Interval", &calc.referenceSyncSettings.referenceSynchronizationInterval,
                    Unparser::UINT32, Parser::UINT32, ValidCondition::POSITIVE_UINT32, Callback::NOTHING,
                    L"Reference Synchronization Interval",
                    L"Sets the synchronization interval between the reference array\n"
                    L"and arbitrary-precision operation when calculating references. When the value is small,\n"
                    L"it guarantees high quality but is slow because the synchronization happens every iterations.\n"
                    L"when the value is high, resulting lower quality but is faster. set 1 to fully sync.");
            window->registerTextInput<uint8_t>(
                    L"Reference Synchronization Radius",
                    &calc.referenceSyncSettings.referenceSynchronizationRadiusPower, Unparser::UINT8, Parser::UINT8,
                    ValidCondition::ALL_UINT8, Callback::NOTHING, L"Reference Synchronization Radius",
                    L"If only the Reference Synchronization Interval is set,\n"
                    L"when the periodic point is reached and it is not a multiple of its value,\n"
                    L"the calculation will be resulted so weird due to significant mismatch with\n"
                    L"arbitrary-precision operations caused by precision loss. This setting determines within the "
                    L"radius : \n"
                    L"10^(-value) from the origin to be considered the periodic point.\n"
                    L"set 0 to fully sync.");

            window->registerCheckboxInput(L"Parallel reference calculation", &calc.useParallelRefCalculation,
                                          Callback::NOTHING, L"Use parallel reference calculation",
                                          L"Sets whether or not the reference calculation should be parallel.\n"
                                          L"It is effective for deep-zoom.");

            window->setWindowCloseFunction([centerPtr, zoomPtr, locationChanged, &calc, &arm] {
                const int exp10 = Perturbator::logZoomToExp10(*zoomPtr);
                if (*locationChanged) {
                    calc.center = fixed_point_complex_i1((*centerPtr)[0], (*centerPtr)[1], exp10);
                    calc.logZoom = *zoomPtr;
                    arm.getRequests().requestRecompute();
                }
                arm.setCurrentSettingsWindows(nullptr);
            });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackFractal::fnIterations(AppRenderManager &arm) {
        return [&arm] {
            auto &calc = arm.getSettings().fractal;
            auto window = std::make_unique<SettingsWindow>(L"Iterations");


            window->registerTextInput<uint64_t>(
                    L"Max Iteration", &calc.maxIteration, Unparser::UINT64, Parser::UINT64, ValidCondition::ALL_UINT64,
                    Callback::NOTHING, L"Set Max Iteration",
                    L"Set maximum iteration. It is disabled when Auto iteration is enabled.");
            window->registerTextInput<uint16_t>(
                    L"Auto Iteration Multiplier", &calc.autoIterationMultiplier, Unparser::UINT16, Parser::UINT16,
                    ValidCondition::ALL_UINT16, Callback::NOTHING, L"Set Auto Iteration Multiplier",
                    L"Set auto iteration multiplier. It is disabled when Auto iteration is disabled.");

            window->registerTextInput<float>(
                    L"Bailout", &calc.bailout, Unparser::FLOAT, Parser::FLOAT,
                    [](const float &v) { return v >= 2 && v <= 8; }, Callback::NOTHING, L"Set Bailout",
                    L"Sets The Bailout radius");
            window->registerRadioButtonInput<FrtDecimalizeIterationMethod>(
                    L"Decimalize iteration", &calc.decimalizeIterationMethod, Callback::NOTHING,
                    L"Decimalize Iteration Method", L"Sets the decimalization method of iterations.");


            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackFractal::fnMpa(AppRenderManager &arm) {
        return [&arm] {
            auto &[minSkipReference, maxMultiplierBetweenLevel, epsilonPower, mpaSelectionMethod,
                   mpaCompressionMethod] = arm.getSettings().fractal.mpaSettings;
            auto window = std::make_unique<SettingsWindow>(L"MP-Approximation");
            window->registerTextInput<uint16_t>(
                    L"Min Skip Reference", &minSkipReference, Unparser::UINT16, Parser::UINT16,
                    [](const unsigned short &v) { return v >= 4; }, Callback::NOTHING, L"Min Skip Reference",
                    L"Set minimum skipping reference iteration when creating a table.");
            window->registerTextInput<uint8_t>(
                    L"Max Multiplier Between Level", &maxMultiplierBetweenLevel, Unparser::UINT8, Parser::UINT8,
                    ValidCondition::POSITIVE_UINT8, Callback::NOTHING,
                    L"Set maximum multiplier between adjacent skipping levels.",
                    L"This means the maximum multiplier of two adjacent periods for the new period that inserts "
                    L"between "
                    L"them,\n"
                    L"So the multiplier between the two periods may in the worst case be the square of this.");
            window->registerTextInput<float>(
                    L"Precision Level", &epsilonPower, Unparser::FLOAT, Parser::FLOAT, ValidCondition::NEGATIVE_FLOAT,
                    Callback::NOTHING, L"Set the precision level based on powers of ten (epsilon).",
                    L"Useful for glitch reduction. you can set this value -15 to -1. if this value is small (-15),\n"
                    L"The fractal will be rendered glitch-less but slow,\n"
                    L"and is large (-1), It will be fast, but maybe shown visible glitches. Default value is -3.");
            window->registerRadioButtonInput<FrtMPASelectionMethod>(
                    L"Selection Method", &mpaSelectionMethod, Callback::NOTHING, L"Set the selection method of MPA.",
                    L"The first target PA is always the front element.");

            window->registerRadioButtonInput<FrtMPACompressionMethod>(
                    L"Compression Method", &mpaCompressionMethod, Callback::NOTHING,
                    L"Set the compression method of MPA.",
                    L"\"Little Compression\" maybe slowing down for table creation, but allocates the memory "
                    L"efficiently.\n"
                    L"\"Strongest\" works based on the Reference Compressor, so if it is disabled, it will behave the "
                    L"same "
                    L"as \"Little Compression\".\n L"
                    L"It uses acceleration when possible, and can accelerate table creation by 10x~100x of times.");


            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<bool()> CallbackFractal::fnGetterAutomaticIterations(AppRenderManager &arm) {
        return [&arm] { return arm.getSettings().fractal.autoMaxIteration; };
    }
    std::function<bool()> CallbackFractal::fnGetterAbsoluteIterationMode(AppRenderManager &arm) {
        return [&arm] { return arm.getSettings().fractal.absoluteIterationMode; };
    }
    std::function<void(bool)> CallbackFractal::fnAutomaticIterations(AppRenderManager &arm) {
        return [&arm](const bool b) { arm.getSettings().fractal.autoMaxIteration = b; };
    }
    std::function<void(bool)> CallbackFractal::fnAbsoluteIterationMode(AppRenderManager &arm) {
        return [&arm](const bool b) { arm.getSettings().fractal.absoluteIterationMode = b; };
    }


} // namespace merutilm::rff2
