//
// Created by Merutilm on 2025-05-14.
//

#include "FnFractal.hpp"

#include "../formula/Perturbator.h"
#include "../settings/Selectable.h"
#include "RFFApplication.hpp"
#include "Utilities.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace merutilm::rff2 {

    void FnFractal::reference(RFFApplication &app) {

        static bool enabled = false;
        auto &frt = app.getSettings().fractal;

        static bool mpfInit = false;
        static mpf_t t;
        if (!mpfInit) {
            mpf_init(t);
            mpfInit = true;
        }

        static std::string real = "";
        static std::string imag = "";
        static float logZoom = 0;
        static std::string realCache = real;
        static std::string imagCache = imag;
        static float logZoomCache = logZoom;
        static bool locationChanged = false;

        if (ImGui::Checkbox("Reference", &enabled)) {
            real = frt.reference.center.real.to_string();
            imag = frt.reference.center.imag.to_string();
            logZoom = frt.general.logZoom;
            realCache = real;
            imagCache = imag;
            logZoomCache = logZoom;
            locationChanged = false;
        }


        if (enabled) {

            ImGui::Begin("Reference");
            ImGui::InputText("Real", &realCache);
            if (ImGui::IsItemDeactivatedAfterEdit()) {


                if (mpf_set_str(t, realCache.data(), 10) == 0) {
                    mpf_abs(t, t);
                    if (mpf_cmp_d(t, Constants::Fractal::MAX_LOC_LEN) < 0) {
                        real = realCache;
                        locationChanged = true;
                    } else {
                        realCache = real;
                    }
                } else {
                    realCache = real;
                }
            }
            Utilities::imguiHelpMarker("Sets the real part of center.");

            ImGui::InputText("Imag", &imagCache);
            if (ImGui::IsItemDeactivatedAfterEdit()) {

                if (mpf_set_str(t, imagCache.data(), 10) == 0) {
                    mpf_abs(t, t);
                    if (mpf_cmp_d(t, Constants::Fractal::MAX_LOC_LEN) < 0) {
                        imag = imagCache;
                        locationChanged = true;
                    } else {
                        imagCache = imag;
                    }
                } else {
                    imagCache = imag;
                }
            }
            Utilities::imguiHelpMarker("Sets the imaginary part of center.");


            if (ImGui::InputFloat("Log Zoom", &logZoomCache)) {
                logZoom = std::max(logZoomCache, Constants::Fractal::ZOOM_MIN);
                locationChanged = true;
            }
            Utilities::imguiHelpMarker("Sets the log scale of zoom.");

            if (ImGui::Button("Load Current Location", ImVec2(-FLT_MIN, 0))) {
                real = frt.reference.center.real.to_string();
                imag = frt.reference.center.imag.to_string();
                logZoom = frt.general.logZoom;
                realCache = real;
                imagCache = imag;
                logZoomCache = logZoom;
                locationChanged = false;
            }

            Utilities::imguiDropdown("Reuse Reference", &frt.reference.reuse);
            Utilities::imguiHelpMarker("Sets the reuse reference method.");

            ImGui::InputScalar("Reference Compression Criteria", ImGuiDataType_U32,
                               &frt.reference.compression.compressCriteria);
            Utilities::imguiHelpMarker(
                    "When compressing references, sets the minimum amount of references to compress at one time.\n"
                    "Reference compression slows down the calculation but frees up memory space.\n"
                    "set 0 to disable.");


            ImGui::InputScalar("Reference Compression Threshold", ImGuiDataType_U8,
                               &frt.reference.compression.compressionThresholdPower);
            Utilities::imguiHelpMarker(
                    "When compressing references, sets the negative exponents of ten of minimum error to be "
                    "considered "
                    "equal.\n"
                    "Reference compression slows down the calculation but frees up memory space.\n"
                    "set 0 to disable.");

            ImGui::Checkbox("NO Compress normalization", &frt.reference.compression.noCompressorNormalization);
            Utilities::imguiHelpMarker("No normalization during reference compression.\n"
                                       "this will accelerates table creation, But may cause table creation to fail in\n"
                                       "the specific locations!!");

            if (ImGui::InputScalar("Reference Synchronization Interval", ImGuiDataType_U32,
                               &frt.reference.sync.referenceSynchronizationInterval)) {
                frt.reference.sync.referenceSynchronizationInterval = std::max(frt.reference.sync.referenceSynchronizationInterval, 1u);
            }
            Utilities::imguiHelpMarker(
                    "Sets the synchronization interval between the reference array\n"
                    "and arbitrary-precision operation when calculating references. When the value is small,\n"
                    "it guarantees high quality but is slow because the synchronization happens every iterations.\n"
                    "when the value is high, resulting lower quality but is faster. set 1 to fully sync.");

            ImGui::InputScalar("Reference Synchronization Radius", ImGuiDataType_U8,
                               &frt.reference.sync.referenceSynchronizationRadiusPower);
            Utilities::imguiHelpMarker(
                    "If only the Reference Synchronization Interval is set,\n"
                    "when the periodic point is reached and it is not a multiple of its value,\n"
                    "the calculation will be resulted so weird due to significant mismatch with\n"
                    "arbitrary-precision operations caused by precision loss. This setting determines within the "
                    "radius : \n"
                    "10^(-value) from the origin to be considered the periodic point.\n"
                    "set 0 to fully sync.");

            ImGui::Checkbox("Parallel reference calculation", &frt.reference.useParallelRefCalculation);
            Utilities::imguiHelpMarker("Sets whether or not the reference calculation should be parallel.\n"
                                       "It is effective for deep-zoom.");



            if (ImGui::Button("Apply Location Changes", ImVec2(-FLT_MIN, 0))) {
                const int exp10 = Perturbator::logZoomToExp10(logZoom);
                if (locationChanged) {
                    frt.reference.center = fixed_point_complex_i1(real, imag, exp10);
                    frt.general.logZoom = logZoom;
                    app.getRequests().requestRecompute();
                }
            }
            if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
                app.getRequests().requestRecompute();
            }

            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                enabled = false;
            }
            ImGui::End();
        }
    }
    void FnFractal::iterations(RFFApplication &app) {

        static bool enabled = false;
        ImGui::Checkbox("Iterations", &enabled);

        if (enabled) {

            ImGui::Begin("Iterations");
            auto &calc = app.getSettings().fractal;
            ImGui::InputScalar("Max Iteration", ImGuiDataType_U64, &calc.perturb.maxIteration);
            Utilities::imguiHelpMarker("Set maximum iteration. It is disabled when Auto iteration is enabled.");


            ImGui::InputScalar("Auto Iteration Multiplier", ImGuiDataType_U16, &calc.perturb.autoIterationMultiplier);
            Utilities::imguiHelpMarker(
                    "Set auto iteration multiplier. It is disabled when Auto iteration is disabled.");

            if (ImGui::InputFloat("Set Bailout", &calc.general.bailout)) {
                calc.general.bailout = std::clamp(calc.general.bailout, 2.f, 8.f);
            }
            Utilities::imguiHelpMarker("Sets The Bailout radius");


            ImGui::InputScalar("Interior Detection Threshold", ImGuiDataType_U8, &calc.perturb.interiorDetectRadiusPower);
            Utilities::imguiHelpMarker(
                    "Set the interior detection threshold. It calculates the distance between the previous and current z at the periodic point.\n"
                    "if the distance is smaller than \"10^-value\", this pixel is set to interior and all subsequent iterations are skipped. Set 0 to disable it.");

            Utilities::imguiDropdown("Decimalize Iteration", &calc.perturb.decimalizeIterationMethod);
            Utilities::imguiHelpMarker("Sets the decimalization method of iterations.");


            if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
                app.getRequests().requestRecompute();
            }
            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                enabled = false;
            }
            ImGui::End();
        }
    }
    void FnFractal::sa(RFFApplication &app) {


        static bool enabled = false;
        ImGui::Checkbox("Series Approximation", &enabled);

        if (enabled) {
            ImGui::Begin("Series Approximation");

            auto &[appliedTermsCount, validatedTermsCount, epsilonPower] = app.getSettings().fractal.sa;
            if (ImGui::InputScalar("Applied Terms", ImGuiDataType_U16, &appliedTermsCount)) {
                appliedTermsCount =
                        std::clamp(appliedTermsCount, static_cast<uint16_t>(1), static_cast<uint16_t>(1024));
            }
            Utilities::imguiHelpMarker("Set the number of terms to approximate the orbit in the first iteration.");


            if (ImGui::InputScalar("Validated Terms Count", ImGuiDataType_U16, &validatedTermsCount)) {
                validatedTermsCount =
                        std::clamp(validatedTermsCount, static_cast<uint16_t>(1), static_cast<uint16_t>(1024));
            }
            Utilities::imguiHelpMarker(
                    "Set the number of terms for the escape condition when generating the series approximation.");

            ImGui::InputFloat("Precision Level", &epsilonPower);
            Utilities::imguiHelpMarker("Useful for glitch reduction. if this value is small,\n"
                                       "The fractal will be rendered glitch-less but slow,\n"
                                       "and is large, It will be fast, but maybe shown visible glitches.");

            if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
                app.getRequests().requestRecompute();
            }

            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                enabled = false;
            }
            ImGui::End();
        }
    }
    void FnFractal::mpa(RFFApplication &app) {

        static bool enabled = false;
        ImGui::Checkbox("MP-Approximation", &enabled);

        if (enabled) {
            ImGui::Begin("MP-Approximation");
            auto &[maxDegree, minSkipReference, maxMultiplierBetweenLevel, epsilonPower, mpaSelectionMethod,
                   mpaCompressionMethod] = app.getSettings().fractal.mpa;
            Utilities::imguiDropdown("Max Degree (Experimental)", &maxDegree);
            Utilities::imguiHelpMarker("Set max degree of PA, Setting this to high value can accelerate the "
                                       "calculation of spiral patterns "
                                       "but slows down the pre-calculation."
                                       " Excluding standard option, it is currently inefficient.");

            if (ImGui::InputScalar("Min Skip Reference", ImGuiDataType_U16, &minSkipReference)) {
                minSkipReference = std::max(minSkipReference, static_cast<uint16_t>(4));
            }
            Utilities::imguiHelpMarker("Set minimum skipping reference iteration when creating a table.");

            if (ImGui::InputScalar("Max Multiplier Between Level", ImGuiDataType_U8, &maxMultiplierBetweenLevel)) {
                maxMultiplierBetweenLevel = std::max(maxMultiplierBetweenLevel, static_cast<uint8_t>(2));
            }
            Utilities::imguiHelpMarker(
                    "Set maximum multiplier between adjacent skipping levels.\n"
                    "This means the maximum multiplier of two adjacent periods for the new period that inserts between "
                    "them,\n"
                    "So the multiplier between the two periods may in the worst case be the square of this.");

            if (ImGui::InputFloat("Precision Level", &epsilonPower)) {
                epsilonPower = std::clamp(epsilonPower, -15.f, -1.f);
            }
            Utilities::imguiHelpMarker(
                    "Set the precision level based on powers of ten (epsilon).\n"
                    "Useful for glitch reduction. you can set this value -15 to -1. if this value is small (-15),\n"
                    "The fractal will be rendered glitch-less but slow,\n"
                    "and is large (-1), It will be fast, but maybe shown visible glitches.");

            Utilities::imguiDropdown("Selection Method", &mpaSelectionMethod);
            Utilities::imguiHelpMarker(
                    "Set the selection method of MPA. The first target PA is always the front element.");

            Utilities::imguiDropdown("Compression Method", &mpaCompressionMethod);
            Utilities::imguiHelpMarker(
                    "Set the compression method of MPA.\n "
                    "\"Little Compression\" maybe slowing down for table creation, but allocates the memory "
                    "efficiently.\n"
                    "\"Strongest\" works based on the Reference Compressor, so if it is disabled, it will behave the "
                    "same "
                    "as \"Little Compression\".\n "
                    "It uses acceleration when possible, and can accelerate table creation.");

            if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
                app.getRequests().requestRecompute();
            }

            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                enabled = false;
            }
            ImGui::End();
        }
    }

    void FnFractal::automaticIterations(RFFApplication &app) {
        ImGui::Checkbox("Auto Iterations", &app.getSettings().fractal.perturb.autoMaxIteration);
    }
    void FnFractal::absoluteIterationMode(RFFApplication &app) {
        ImGui::Checkbox("Absolute Iteration Mode", &app.getSettings().fractal.perturb.absoluteIterationMode);
    }


} // namespace merutilm::rff2
