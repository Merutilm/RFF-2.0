//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFractal.hpp"

#include "../formula/Perturbator.h"
#include "Callback.hpp"
#include "SettingsMenuGenerator.hpp"

namespace merutilm::rff2 {

#ifdef _WIN32
    std::function<void()> CallbackFractal::fnReference(AppRenderManager &arm) {
        return [&arm] {
            auto &frt = arm.getSettings().fractal;
            auto window = std::make_unique<SettingsWindow>("Reference");

            auto centerPtr = std::make_shared<std::array<std::string, 2>>();
            *centerPtr = {frt.reference.center.real.to_string(), frt.reference.center.imag.to_string()};

            auto zoomPtr = std::make_shared<float>(frt.general.logZoom);
            auto locationChanged = std::make_shared<bool>(false);


            window->registerTextInput<std::string>(
                    "Real", &(*centerPtr)[0], Unparser::STRING, Parser::STRING,
                    [](const std::string &v) {
                        mpf_t t;
                        const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                        mpf_clear(t);
                        return valid;
                    },
                    [centerPtr, locationChanged] { *locationChanged = true; }, "Real",
                    "Sets the real part of center.");
            window->registerTextInput<std::string>(
                    "Imag", &(*centerPtr)[1], Unparser::STRING, Parser::STRING,
                    [](const std::string &v) {
                        mpf_t t;
                        const bool valid = mpf_init_set_str(t, v.data(), 10) == 0;
                        mpf_clear(t);
                        return valid;
                    },
                    [centerPtr, locationChanged] { *locationChanged = true; }, "Imag",
                    "Sets the imaginary part of center.");
            window->registerTextInput<float>(
                    "Log Zoom", zoomPtr.get(), Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                    [zoomPtr, locationChanged] { *locationChanged = true; }, "Log zoom",
                    "Sets the log scale of zoom.");
            window->registerRadioButtonInput<FrtReferenceReuseMethod>("Reuse Reference", &frt.reference.reuse,
                                                                      Callback::NOTHING, "Reuse Reference method",
                                                                      "Sets the reuse reference method.");
            window->registerTextInput<uint32_t>(
                    "Reference Compression Criteria", &frt.reference.compression.compressCriteria, Unparser::UINT32,
                    Parser::UINT32, ValidCondition::ALL_UINT32, Callback::NOTHING, "Reference Compression Criteria",
                    "When compressing references, sets the minimum amount of references to compress at one time.\n"
                    "Reference compression slows down the calculation but frees up memory space.\n"
                    "set 0 to disable.");
            window->registerTextInput<uint8_t>(
                    "Reference Compression Threshold", &frt.reference.compression.compressionThresholdPower,
                    Unparser::UINT8, Parser::UINT8, ValidCondition::ALL_UINT8, Callback::NOTHING,
                    "Reference Compression Threshold Power",
                    "When compressing references, sets the negative exponents of ten of minimum error to be "
                    "considered "
                    "equal.\n"
                    "Reference compression slows down the calculation but frees up memory space.\n"
                    "set 0 to disable.");
            window->registerCheckboxInput(
                    "NO Compressor normalization", &frt.reference.compression.noCompressorNormalization,
                    Callback::NOTHING, "NO Compressor normalization",
                    "No normalization during reference compression.\n"
                    "this will accelerates table creation, But may cause table creation to fail in\n"
                    "the specific locations!!");
            window->registerTextInput<uint32_t>(
                    "Reference Synchronization Interval", &frt.reference.sync.referenceSynchronizationInterval,
                    Unparser::UINT32, Parser::UINT32, ValidCondition::POSITIVE_UINT32, Callback::NOTHING,
                    "Reference Synchronization Interval",
                    "Sets the synchronization interval between the reference array\n"
                    "and arbitrary-precision operation when calculating references. When the value is small,\n"
                    "it guarantees high quality but is slow because the synchronization happens every iterations.\n"
                    "when the value is high, resulting lower quality but is faster. set 1 to fully sync.");
            window->registerTextInput<uint8_t>(
                    "Reference Synchronization Radius",
                    &frt.reference.sync.referenceSynchronizationRadiusPower, Unparser::UINT8, Parser::UINT8,
                    ValidCondition::ALL_UINT8, Callback::NOTHING, "Reference Synchronization Radius",
                    "If only the Reference Synchronization Interval is set,\n"
                    "when the periodic point is reached and it is not a multiple of its value,\n"
                    "the calculation will be resulted so weird due to significant mismatch with\n"
                    "arbitrary-precision operations caused by precision loss. This setting determines within the "
                    "radius : \n"
                    "10^(-value) from the origin to be considered the periodic point.\n"
                    "set 0 to fully sync.");

            window->registerCheckboxInput("Parallel reference calculation", &frt.reference.useParallelRefCalculation,
                                          Callback::NOTHING, "Use parallel reference calculation",
                                          "Sets whether or not the reference calculation should be parallel.\n"
                                          "It is effective for deep-zoom.");

            window->setWindowCloseFunction([centerPtr, zoomPtr, locationChanged, &frt, &arm] {
                const int exp10 = Perturbator::logZoomToExp10(*zoomPtr);
                if (*locationChanged) {
                    frt.reference.center = fixed_point_complex_i1((*centerPtr)[0], (*centerPtr)[1], exp10);
                    frt.general.logZoom = *zoomPtr;
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
            auto window = std::make_unique<SettingsWindow>("Iterations");


            window->registerTextInput<uint64_t>(
                    "Max Iteration", &calc.perturb.maxIteration, Unparser::UINT64, Parser::UINT64, ValidCondition::ALL_UINT64,
                    Callback::NOTHING, "Set Max Iteration",
                    "Set maximum iteration. It is disabled when Auto iteration is enabled.");
            window->registerTextInput<uint16_t>(
                    "Auto Iteration Multiplier", &calc.perturb.autoIterationMultiplier, Unparser::UINT16, Parser::UINT16,
                    ValidCondition::ALL_UINT16, Callback::NOTHING, "Set Auto Iteration Multiplier",
                    "Set auto iteration multiplier. It is disabled when Auto iteration is disabled.");

            window->registerTextInput<float>(
                    "Bailout", &calc.general.bailout, Unparser::FLOAT, Parser::FLOAT,
                    [](const float &v) { return v >= 2 && v <= 8; }, Callback::NOTHING, "Set Bailout",
                    "Sets The Bailout radius");
            window->registerRadioButtonInput<FrtDecimalizeIterationMethod>(
                    "Decimalize iteration", &calc.perturb.decimalizeIterationMethod, Callback::NOTHING,
                    "Decimalize Iteration Method", "Sets the decimalization method of iterations.");


            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackFractal::fnSa(AppRenderManager &arm) {
        return [&arm] {
            auto &[appliedTermsCount, validatedTermsCount, epsilonPower] = arm.getSettings().fractal.sa;
            auto window = std::make_unique<SettingsWindow>("Series Approximation");
            window->registerTextInput<uint16_t>(
                    "Applied Terms Count", &appliedTermsCount, Unparser::UINT16, Parser::UINT16,
                    [](const unsigned short &v) { return v >= 1 && v <= 1024; }, Callback::NOTHING, "Applied Terms Count",
                    "Set the number of terms to approximate the orbit in the first iteration.");
            window->registerTextInput<uint16_t>(
                    "Validated Terms Count", &validatedTermsCount, Unparser::UINT16, Parser::UINT16,
                    [](const unsigned short &v) { return v >= 1 && v <= 1024; }, Callback::NOTHING,
                    "Validated Terms Count",
                    "Set the number of terms for the escape condition when generating the series approximation.");
            window->registerTextInput<float>(
                    "Precision Level", &epsilonPower, Unparser::FLOAT, Parser::FLOAT, ValidCondition::NEGATIVE_FLOAT,
                    Callback::NOTHING, "Set the precision level based on powers of ten (epsilon).",
                    "Useful for glitch reduction. you can set this value -15 to -1. if this value is small (-15),\n"
                    "The fractal will be rendered glitch-less but slow,\n"
                    "and is large (-1), It will be fast, but maybe shown visible glitches.");

            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackFractal::fnMpa(AppRenderManager &arm) {
        return [&arm] {
            auto &[maxDegree, minSkipReference, maxMultiplierBetweenLevel, epsilonPower, mpaSelectionMethod,
                   mpaCompressionMethod] = arm.getSettings().fractal.mpa;
            auto window = std::make_unique<SettingsWindow>("MP-Approximation");
            window->registerSelectionInput<FrtMPADegree>(
                    "Max Degree (Experimental)", &maxDegree, Callback::NOTHING, "Max Degree",
                    "Set max degree of PA, Setting this to high value can accelerate the calculation of spiral patterns but slows down the pre-calculation."
                    " Excluding standard option, it is currently inefficient.");
            window->registerTextInput<uint16_t>(
                    "Min Skip Reference", &minSkipReference, Unparser::UINT16, Parser::UINT16,
                    [](const unsigned short &v) { return v >= 4; }, Callback::NOTHING, "Min Skip Reference",
                    "Set minimum skipping reference iteration when creating a table.");
            window->registerTextInput<uint8_t>(
                    "Max Multiplier Between Level", &maxMultiplierBetweenLevel, Unparser::UINT8, Parser::UINT8,
                    ValidCondition::POSITIVE_UINT8, Callback::NOTHING,
                    "Set maximum multiplier between adjacent skipping levels.",
                    "This means the maximum multiplier of two adjacent periods for the new period that inserts "
                    "between "
                    "them,\n"
                    "So the multiplier between the two periods may in the worst case be the square of this.");
            window->registerTextInput<float>(
                    "Precision Level", &epsilonPower, Unparser::FLOAT, Parser::FLOAT, ValidCondition::NEGATIVE_FLOAT,
                    Callback::NOTHING, "Set the precision level based on powers of ten (epsilon).",
                    "Useful for glitch reduction. you can set this value -15 to -1. if this value is small (-15),\n"
                    "The fractal will be rendered glitch-less but slow,\n"
                    "and is large (-1), It will be fast, but maybe shown visible glitches.");
            window->registerRadioButtonInput<FrtMPASelectionMethod>(
                    "Selection Method", &mpaSelectionMethod, Callback::NOTHING, "Set the selection method of MPA.",
                    "The first target PA is always the front element.");

            window->registerRadioButtonInput<FrtMPACompressionMethod>(
                    "Compression Method", &mpaCompressionMethod, Callback::NOTHING,
                    "Set the compression method of MPA.",
                    "\"Little Compression\" maybe slowing down for table creation, but allocates the memory "
                    "efficiently.\n"
                    "\"Strongest\" works based on the Reference Compressor, so if it is disabled, it will behave the "
                    "same "
                    "as \"Little Compression\".\n "
                    "It uses acceleration when possible, and can accelerate table creation by 10x~100x of times.");


            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
#endif
    std::function<bool()> CallbackFractal::fnGetterAutomaticIterations(RFFApplication &arm) {
        return [&arm] { return arm.getSettings().fractal.perturb.autoMaxIteration; };
    }
    std::function<bool()> CallbackFractal::fnGetterAbsoluteIterationMode(RFFApplication &arm) {
        return [&arm] { return arm.getSettings().fractal.perturb.absoluteIterationMode; };
    }
    std::function<void(bool)> CallbackFractal::fnAutomaticIterations(RFFApplication &arm) {
        return [&arm](const bool b) { arm.getSettings().fractal.perturb.autoMaxIteration = b; };
    }
    std::function<void(bool)> CallbackFractal::fnAbsoluteIterationMode(RFFApplication &arm) {
        return [&arm](const bool b) { arm.getSettings().fractal.perturb.absoluteIterationMode = b; };
    }


} // namespace merutilm::rff2