//
// Created by Merutilm on 2025-05-14.
//

#include "FnRender.hpp"

#include "RFFApplication.hpp"
#include "Utilities.h"
#include "imgui.h"

namespace merutilm::rff2 {

    void FnRender::setResolutionProperties(RFFApplication &app) {
        static bool setResolution = false;
        static std::array<uint32_t, 2> resolutionTemp;
        static float clarityMultiplierTemp;
        static bool valueChanged = false;
        auto [width, height] = app.getWindowContext().getSwapchain().getSwapchainExtent();

        if (ImGui::Checkbox("Set Resolution Properties", &setResolution)) {
            resolutionTemp[0] = width;
            resolutionTemp[1] = height;
            clarityMultiplierTemp = app.getSettings().render.clarityMultiplier;
            valueChanged = false;
        }

        if (setResolution) {

            auto &[clarityMultiplier, fps, linearInterpolation, threads] = app.getSettings().render;

            ImGui::Begin("Set Resolution Properties");

            if (ImGui::InputScalarN("Window size", ImGuiDataType_U32, resolutionTemp.data(), 2)) {
                resolutionTemp[0] = std::max(resolutionTemp[0], Constants::Render::MIN_WINDOW_WIDTH);
                resolutionTemp[1] = std::max(resolutionTemp[1], Constants::Render::MIN_WINDOW_HEIGHT);
                valueChanged = true;
            }

            if (ImGui::DragFloat("Clarity Multiplier", &clarityMultiplierTemp,
                                 Constants::UI::CLARITY_MULTIPLIER_UNIT, Constants::Render::MIN_CLARITY_MULTIPLIER,
                                 Constants::Render::MAX_CLARITY_MULTIPLIER)) {
                clarityMultiplierTemp = std::clamp(clarityMultiplierTemp, Constants::Render::MIN_CLARITY_MULTIPLIER,
                                                   Constants::Render::MAX_CLARITY_MULTIPLIER);\
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
                clarityMultiplier = clarityMultiplierTemp;
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

    void FnRender::setRenderProperties(RFFApplication &app) {
        static bool setRenderProperties = false;

        ImGui::Checkbox("Set Render Properties", &setRenderProperties);

        if (setRenderProperties) {

            auto &[clarityMultiplier, fps, linearInterpolation, threads] = app.getSettings().render;


            constexpr uint32_t minThread = 1;

            const uint32_t maxThreads = std::thread::hardware_concurrency();

            ImGui::Begin("Set Render Properties");

            if (ImGui::SliderFloat("Framerate", &fps, Constants::Render::MIN_FPS, Constants::Render::MAX_FPS)) {
                fps = std::clamp(fps, Constants::Render::MIN_FPS, Constants::Render::MAX_FPS);
                app.getWindowContext().getWindow()->initializerSettings.framerate = fps;
            }
            Utilities::imguiHelpMarker("Sets the Framerate.");


            if (ImGui::SliderScalar("Threads", ImGuiDataType_U32, &threads, &minThread, &maxThreads)) {
                // noop
            }
            Utilities::imguiHelpMarker("Sets the number of threads while rendering an image.");

            if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
                setRenderProperties = false;
            }

            ImGui::End();
        }
    }

    void FnRender::linearInterpolation(RFFApplication &app) {
        if (ImGui::Checkbox("Linear Interpolation", &app.getSettings().render.linearInterpolation)) {
            app.getRequests().requestShader();
        }
    }


} // namespace merutilm::rff2
