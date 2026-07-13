//
// Created by Merutilm on 2025-05-14.
//

#include "FnRender.hpp"

#include "RFFApplication.hpp"
#include "Utilities.h"
#include "imgui.h"

namespace merutilm::rff2 {

    void FnRender::setRenderProperties(RFFApplication &app) {
        static bool setRenderProperties = false;
        ImGui::Checkbox("Set Render Properties", &setRenderProperties);
        if (setRenderProperties) {

            auto &[clarityMultiplier, fps, linearInterpolation, threads] = app.getSettings().render;
            constexpr uint32_t minThread = 1;
            const uint32_t maxThreads = std::thread::hardware_concurrency();

            ImGui::Begin("Set Render Properties");

            if (ImGui::DragFloat("Clarity", &clarityMultiplier, 0.125f, 0.125f, 4)) {
                clarityMultiplier = std::clamp(clarityMultiplier, 0.125f, 4.0f);
                app.getRequests().requestResize(app.rootWindowContext->getSwapchain().getSwapchainExtent());
            }

            Utilities::imguiHelpMarker("Sets the clarity multiplier.");
            if (ImGui::SliderFloat("Framerate", &fps, 30, 240)) {
                fps = std::clamp(fps, 30.0f, 240.0f);
                app.getWindowContext().getWindow()->initializerSettings.framerate = fps;
            }
            Utilities::imguiHelpMarker("Sets the Framerate.");


            if (ImGui::SliderScalar("Threads", ImGuiDataType_U32, &threads, &minThread, &maxThreads)) {
                //noop
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
