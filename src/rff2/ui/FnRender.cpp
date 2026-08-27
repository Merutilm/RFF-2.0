//
// Created by Merutilm on 2025-05-14.
//

#include "FnRender.hpp"

#include "RFF2.hpp"
#include "Utilities.h"
#include "imgui.h"

namespace merutilm::rff2 {

    void FnRender::setResolutionProperties(RFF2 &app) {
        static bool setResolution = false;
        static std::array<uint32_t, 2> resolutionTemp;
        static float clarityMultiplierTemp;
        static bool valueChanged = false;
        auto [width, height] = app.getWindowContext().getSwapchain().getSwapchainExtent();
        float &clarityMultiplier = app.getSettings().render.clarityMultiplier;


        if (ImGui::Checkbox("Set Resolution Properties", &setResolution)) {
            resolutionTemp[0] = width;
            resolutionTemp[1] = height;
            clarityMultiplierTemp = clarityMultiplier;
            valueChanged = false;
        }

        if (setResolution) {


            ImGui::Begin("Set Resolution Properties");

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
                app.getSettings().render.clarityMultiplier = clarityMultiplierTemp;
                app.getRequests().requestResize(swapchainExtent);
                valueChanged = false;
            } else if (!valueChanged)
                ImGui::EndDisabled();

            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                setResolution = false;
            }

            ImGui::End();
        }
    }

    void FnRender::setRenderProperties(RFF2 &app) {
        static bool setRenderProperties = false;

        ImGui::Checkbox("Set Render Properties", &setRenderProperties);

        if (setRenderProperties) {

            float &fps = app.getSettings().render.fps;


            constexpr uint32_t minThread = 1;

            const uint32_t maxThreads = std::thread::hardware_concurrency();

            ImGui::Begin("Set Render Properties");

            if (ImGui::SliderFloat("Framerate", &fps, Constants::Render::MIN_FPS, Constants::Render::MAX_FPS)) {
                fps = std::clamp(fps, Constants::Render::MIN_FPS, Constants::Render::MAX_FPS);
                app.getWindowContext().getWindow()->initializerSettings.framerate = fps;
            }
            Utilities::imguiHelpMarker("Sets the Framerate.");


            if (ImGui::SliderScalar("Threads", ImGuiDataType_U32, &app.getSettings().fractal.general.threads,
                                    &minThread, &maxThreads)) {
                // noop
            }
            Utilities::imguiHelpMarker("Sets the number of threads while rendering an image.");

            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                setRenderProperties = false;
            }

            ImGui::End();
        }
    }

    void FnRender::setComputeShader(RFF2 &app) {
        static bool setComputeShader = false;

        if (!app.engine->getCore().getPhysicalDeviceLoader().getPhysicalDeviceFeatures().shaderInt64)
            return;

        ImGui::Checkbox("Compute Shader", &setComputeShader);
        if (setComputeShader) {

            ImGui::Begin("Compute Shader");

            auto &[use, mpaMode, batchSize, allowedGlitchPixelCount] = app.getSettings().render.computeShader;

            if (app.engine->getCore().getPhysicalDeviceLoader().getPhysicalDeviceFeatures().shaderInt64) {
                if (ImGui::Checkbox("Use", &use)) {
                    // noop
                }
                Utilities::imguiHelpMarker("Use Compute shader instead of multithreading. "
                                           "it is only available for single-precision values down to 1e-35, "
                                           "uncompressed MP table and uncompressed reference.");

                Utilities::imguiDropdown("MPA Mode", &mpaMode);
                Utilities::imguiHelpMarker(
                        "Sets MPA Mode. finding appropriate pa from mp-table on gpu-level is so expensive.");

                if (ImGui::InputScalar("Batch Size", ImGuiDataType_U32, &batchSize)) {
                    batchSize = std::clamp(batchSize, 1024u, 16777216u);
                }
                Utilities::imguiHelpMarker("Sets the absolute iteration batch size of compute shader. "
                                           "This prevents the graphics driver forces reset gpu if a process takes too long to complete.");

                if (ImGui::InputScalar("Allowed Glitch Pixel Count", ImGuiDataType_U32, &allowedGlitchPixelCount)) {
                    allowedGlitchPixelCount = std::max(allowedGlitchPixelCount, 0u);
                }
                Utilities::imguiHelpMarker("If a few pixels are abnormally iterated, skip those pixels.");
            }
            if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
                app.getRequests().requestRecompute();
            }
            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                setComputeShader = false;
            }

            ImGui::End();
        }
    }


} // namespace merutilm::rff2
