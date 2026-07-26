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
            auto [width, height] = app.getWindowContext().getSwapchain().getSwapchainExtent();
            std::array resRaw{static_cast<int>(width), static_cast<int>(height)};

            constexpr uint32_t minThread = 1;

            const uint32_t maxThreads = std::thread::hardware_concurrency();

            ImGui::Begin("Set Render Properties");

            if (ImGui::DragFloat("Clarity", &clarityMultiplier, 0.125f, 0.125f, 4)) {
                clarityMultiplier = std::clamp(clarityMultiplier, 0.125f, 4.0f);
                app.getRequests().requestResize(app.rootWindowContext->getSwapchain().getSwapchainExtent());
            }

            if (ImGui::InputInt2("Resolution", resRaw.data())) {
                resRaw[0] = std::max(resRaw[0], Constants::Render::MIN_WINDOW_WIDTH);
                resRaw[1] = std::max(resRaw[1], Constants::Render::MIN_WINDOW_HEIGHT);
                app.getWindowContext().getWindow()->setResolution(resRaw[0], resRaw[1]);
            }


            Utilities::imguiHelpMarker("Sets the clarity multiplier.");
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
