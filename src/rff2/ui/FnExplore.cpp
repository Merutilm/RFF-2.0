//
// Created by Merutilm on 2025-05-16.
//

#include "FnExplore.hpp"

#include <format>

#include <cassert>
#include "Utilities.h"

#include "../constants/Constants.hpp"
#include "../locator/MB2Locator.h"
#include "RFFApplication.hpp"

namespace merutilm::rff2 {


    void FnExplore::recompute(RFFApplication &app) {
        if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
            return app.getRequests().requestRecompute();
        }
    };
    void FnExplore::reset(RFFApplication &app) {

        if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 0))) {
            app.getRequests().requestDefaultSettings();
            app.getRequests().requestResize(app.rootWindowContext->getSwapchain().getSwapchainExtent());

            app.getRequests().requestShader();
            app.getRequests().requestRecompute();
        }
    }
    void FnExplore::cancelRender(RFFApplication &app) {
        if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))) {
            app.getState().cancel();
        }
    }
    void FnExplore::findCenter(RFFApplication &app) {

        if (ImGui::Button("Find Center", ImVec2(-FLT_MIN, 0))) {
            const MB2RenderDataBase *data = app.getCurrentRenderData();
            if (data == nullptr || !data->getReference())
                return;

            if (const std::unique_ptr<fixed_point_complex_i1> c = MB2Locator::findCenter(*data); c == nullptr) {
                vkh::logger::log_warn("No center found!");
            } else {
                app.getSettings().fractal.reference.center = *c;
                app.getRequests().requestRecompute();
            }
        }
    }

    void FnExplore::locateMinibrot(RFFApplication &app) {

        if (ImGui::Button("Locate Minibrot", ImVec2(-FLT_MIN, 0))) {
            Settings &settings = app.getSettings();

            if (settings.fractal.reference.reuse != FrtReferenceReuseMethod::DISABLED) {
                vkh::logger::log_warn("Do not reuse reference!");
                return;
            }

            app.getState().cancel();
            const MB2RenderDataBase *data = app.getCurrentRenderData();
            if (data == nullptr) {
                throw vkh::exception_invalid_state("Perturbator cannot be null");
            }

            app.getState().createThread([&app, data,
                                         &settings] {
                const auto ref = data->getReference();

                if (ref == nullptr) {
                    vkh::logger::log_warn("Please wait until the calculation is complete.", "Caution");
                    return;
                }

                const uint64_t longestPeriod = ref->longestPeriod();

                const std::unique_ptr<MB2Locator> locator = MB2Locator::locateMinibrot(
                        app.getState(), data, getActionWhileFindingMBCenter(app, longestPeriod),
                        getActionWhileCreatingTable(app), getActionWhileFindingZoom(app));

                if (locator == nullptr) {
                    vkh::logger::log("Locate Minibrot Cancelled.");
                    return;
                }
                const FractalSettings &locatorCalc = locator->data->fractalSettings;
                settings.fractal.reference.center = locatorCalc.reference.center;
                settings.fractal.general.logZoom = locatorCalc.general.logZoom - MB2Locator::MINIBROT_LOG_ZOOM_OFFSET;
                app.getRequests().requestRecompute();
            });
        }
    }

    std::function<void(uint64_t, int)> FnExplore::getActionWhileFindingMBCenter(RFFApplication &app,
                                                                                      const uint64_t longestPeriod) {
        return [&app, longestPeriod](const uint64_t p, int i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("L : {:.3f}%[{}]",
                                                 static_cast<float>(100 * p) / static_cast<float>(longestPeriod), i));
            }
        };
    }

    std::function<void(uint64_t, float)> FnExplore::getActionWhileCreatingTable(RFFApplication &app) {
        return [&app](const uint64_t, const float i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format("A : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(float)> FnExplore::getActionWhileFindingZoom(RFFApplication &app) {
        return [&app](float zoom) {
            app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format("Z : 10^{}", zoom));
        };
    }
} // namespace merutilm::rff2
