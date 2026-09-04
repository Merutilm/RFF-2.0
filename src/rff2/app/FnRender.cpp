//
// Created by Merutilm on 2025-05-14.
//

#include "FnRender.hpp"

#include "RFF2.hpp"
#include "Utilities.h"
#include "imgui.h"

namespace merutilm::rff2 {

    void FnRender::setResolutionProperties(RFF2 &app) {

        auto [width, height] = app.getWindowContext().getSwapchain().getSwapchainExtent();
        const float clarityMultiplier = app.getSettings().render.display.clarityMultiplier;
        static std::array resolutionTemp{width, height};
        static float clarityMultiplierTemp = clarityMultiplier;
        static bool valueChanged = false;

        if (ImGui::TreeNode("Set Resolution Properties")) {

            if (ImGui::InputScalarN("Window size", ImGuiDataType_U32, resolutionTemp.data(), 2)) {
                resolutionTemp[0] = std::max(resolutionTemp[0], Constants::Render::MIN_WINDOW_WIDTH);
                resolutionTemp[1] = std::max(resolutionTemp[1], Constants::Render::MIN_WINDOW_HEIGHT);
                valueChanged = true;
            }

            if (ImGui::DragFloat("Clarity Multiplier", &clarityMultiplierTemp, Constants::UI::CLARITY_MULTIPLIER_UNIT,
                                 Constants::Render::MIN_CLARITY_MULTIPLIER,
                                 Constants::Render::MAX_CLARITY_MULTIPLIER)) {
                clarityMultiplierTemp = std::clamp(clarityMultiplierTemp, Constants::Render::MIN_CLARITY_MULTIPLIER,
                                                   Constants::Render::MAX_CLARITY_MULTIPLIER);
                valueChanged = true;
            }

            if (ImGui::Button("Load Current", ImVec2(-FLT_MIN, 0))) {
                resolutionTemp[0] = width;
                resolutionTemp[1] = height;
                clarityMultiplierTemp = clarityMultiplier;
                valueChanged = false;
            }


            const VkExtent2D swapchainExtent = {resolutionTemp[0], resolutionTemp[1]};
            const VkExtent2D calcExtent = RendererUtils::getInternalImageExtent(swapchainExtent, clarityMultiplierTemp);

            const std::string str = std::format("Apply {:d} x {:d}", calcExtent.width, calcExtent.height);

            if (!valueChanged)
                ImGui::BeginDisabled();

            if (ImGui::Button(str.data(), ImVec2(-FLT_MIN, 0))) {
                app.getWindowContext().getWindow()->setResolution(static_cast<int>(resolutionTemp[0]),
                                                                  static_cast<int>(resolutionTemp[1]));
                app.getSettings().render.display.clarityMultiplier = clarityMultiplierTemp;
                app.getRequests().requestResize(swapchainExtent);
                valueChanged = false;
            } else if (!valueChanged)
                ImGui::EndDisabled();


            ImGui::TreePop();
        }
    }

    void FnRender::setRenderProperties(RFF2 &app) {

        if (ImGui::TreeNode("Set Render Properties")) {

            float &fps = app.getSettings().render.display.fps;


            constexpr uint32_t minThread = 1;

            const uint32_t maxThreads = std::thread::hardware_concurrency();

            if (ImGui::SliderFloat("Framerate", &fps, Constants::Render::MIN_FPS, Constants::Render::MAX_FPS)) {
                fps = std::clamp(fps, Constants::Render::MIN_FPS, Constants::Render::MAX_FPS);
                app.getWindowContext().getWindow()->initializerSettings.framerate = fps;
            }
            Utilities::imguiHelpMarker("Sets the Framerate.");

            if (Utilities::imguiDropdown("Pixel Render Priority", &app.getSettings().render.display.pixelRenderPriority)) {
                // noop
            }


            if (ImGui::SliderScalar("Threads", ImGuiDataType_U32, &app.getSettings().fractal.general.threads,
                                    &minThread, &maxThreads)) {
                // noop
            }
            Utilities::imguiHelpMarker("Sets the number of threads while rendering an image.");

            ImGui::TreePop();
        }
    }

    void FnRender::setComputeShader(RFF2 &app) {

        if (!app.engine->getCore().getPhysicalDeviceLoader().getPhysicalDeviceFeatures().shaderInt64)
            return;

        if (ImGui::TreeNode("Compute Shader")) {

            auto &[use, preferredBatchDuration, allowedGlitchPixelCount,  completelyIgnoreMpa, automaticAcceptMpaBatches, interpolateIsolated] = app.getSettings().render.computeShader;

            if (app.engine->getCore().getPhysicalDeviceLoader().getPhysicalDeviceFeatures().shaderInt64) {
                if (ImGui::Checkbox("Use", &use)) {
                    // noop
                }
                Utilities::imguiHelpMarker("Use Compute shader instead of multithreading. "
                                           "it is only available for single-precision values down to 1e-35, "
                                           "uncompressed MP table and uncompressed reference.");


                if (ImGui::InputFloat("Preferred Batch Duration", &preferredBatchDuration)) {
                    preferredBatchDuration = std::clamp(preferredBatchDuration, 0.01f, 10.f);
                }
                Utilities::imguiHelpMarker("Sets the preferred batch duration of compute shader. "
                                           "The batch size starts at 128 and is doubled when the dispatch time is shorter than this duration.");

                if (ImGui::InputScalar("Allowed Glitch Pixel Count", ImGuiDataType_U32, &allowedGlitchPixelCount)) {
                    allowedGlitchPixelCount = std::max(allowedGlitchPixelCount, 0u);
                }
                Utilities::imguiHelpMarker("If a few pixels are abnormally iterated, skip those pixels.");

                ImGui::Checkbox("Completely Ignore MP-Approx", &completelyIgnoreMpa);
                Utilities::imguiHelpMarker(
                        "Ignores MPA. finding appropriate pa from mp-table on gpu-level is so expensive.");

                if (completelyIgnoreMpa) {
                    ImGui::BeginDisabled();
                }
                ImGui::InputScalar("Automatic Accept Batches", ImGuiDataType_U32, &automaticAcceptMpaBatches);
                Utilities::imguiHelpMarker("MP-Approximation is automatically used for the first few batches. After that, it is no longer used. set 0 to fully use.");

                if (completelyIgnoreMpa) {
                    ImGui::EndDisabled();
                }

                ImGui::Checkbox("Interpolate Isolated Pixel", &interpolateIsolated);
            }

            ImGui::TreePop();
        }
    }


} // namespace merutilm::rff2
