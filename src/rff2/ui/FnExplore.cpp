//
// Created by Merutilm on 2025-05-16.
//

#include "FnExplore.hpp"

#include <format>

#include <cassert>
#include "Utilities.h"

#include "../constants/Constants.hpp"
#include "../mb/MB2Locator.h"
#include "RFF2.hpp"

namespace merutilm::rff2 {


    void FnExplore::recompute(RFF2 &app) {
        if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
            return app.getRequests().requestRecompute();
        }
    };
    void FnExplore::reset(RFF2 &app) {

        if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 0))) {
            app.getRequests().requestDefaultSettings();
            app.getRequests().requestResize(app.rootWindowContext->getSwapchain().getSwapchainExtent());

            app.getRequests().requestShader();
            app.getRequests().requestRecompute();
        }
    }
    void FnExplore::cancelRender(RFF2 &app) {
        if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))) {
            app.getState().cancel();
        }
    }
    void FnExplore::setCursorToCenter(RFF2 &app) {
        ImGui::Checkbox("Auto Set Cursor To Center", &app.getSettings().explore.setCursorToCenter);
    }
    void FnExplore::reuseReferenceDetail(RFF2 &app) {
        auto& frt = app.getSettings().fractal;
        ImGui::Checkbox("Reuse Reference", &frt.reference.reuse);

        MB2RenderDataBase * renderData = app.getCurrentRenderData();

        if (frt.reference.reuse && renderData && renderData->getReference()) {
            if (ImGui::Button("Go to Original Reference", ImVec2(-FLT_MIN, 0))) {
                frt.reference.center = renderData->getReference()->center;
                frt.general.logZoom = renderData->getReference()->logZoom;
                renderData->translate(frt.general.logZoom, renderData->getReference()->dcMax, app.getSettings().fractal.perturb, frt.reference.center, getActionWhileSeriesApprox(app));
                app.getRequests().requestRecompute();
            }
        }
    }
    void FnExplore::locateCenteredReference(RFF2 &app) {

        std::unique_ptr<MB2RenderDataBase> &data = app.getCurrentRenderDataOwnRef();
                    Settings &settings = app.getSettings();
        if (data && data->getReference() && data->getPerturbator() && !settings.fractal.reference.reuse) {
            if (ImGui::Button("Locate Centered Reference", ImVec2(-FLT_MIN, 0))) {

                ParallelRenderState &state = app.getState();

                state.createThread([&] {
                    const float startTime = app.rootWindowContext->getWindow()->getTime();

                    uint64_t period = data->getReference()->longestPeriod();
                    const auto center = MB2Locator::locateMinibrot(
                            state, *data, *app.getApproxTableCache(), getActionWhileFindingMBCenter(app, period),
                            getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app),
                            getActionWhileFindingZoom(app));
                    if (center == nullptr)
                        return;

                    FractalSettings refCalc = settings.fractal;
                    refCalc.reference.center = center->data->fractalSettings.reference.center;
                    refCalc.general.logZoom = center->data->fractalSettings.general.logZoom - MB2Locator::MINIBROT_LOG_ZOOM_OFFSET;
                    int refExp10 = Perturbator::logZoomToExp10(refCalc.general.logZoom);
                    if (refCalc.general.logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                        data = std::make_unique<DeepMB2RenderData>(
                                state, refCalc, *app.getApproxTableCache(), center->data->getPerturbator()->dcMax, refExp10,
                                data->getReference()->length(), period, getActionWhileRefCalc(app, startTime), getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app), false);
                    } else {
                        data = std::make_unique<LightMB2RenderData>(
                                state, refCalc, *app.getApproxTableCache(), center->data->getPerturbator()->dcMax, refExp10,
                                data->getReference()->length(), period, getActionWhileRefCalc(app, startTime), getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app), false);
                    }

                    settings.fractal.reference.reuse = true;
                    app.getRequests().requestRecompute();
                });
            }
        }
    }

    void FnExplore::locateMinibrot(RFF2 &app) {

        Settings &settings = app.getSettings();
        if (!settings.fractal.reference.reuse) {
            if (ImGui::Button("Locate Minibrot", ImVec2(-FLT_MIN, 0))) {

                app.getState().cancel();
                const MB2RenderDataBase *data = app.getCurrentRenderData();
                std::unique_ptr<ApproxTableCacheBase> *cache = app.getApproxTableCache();
                if (!data || !cache) {
                    throw vkh::exception_invalid_state("Perturbator cannot be null");
                }

                app.getState().createThread([&app, data, cache, &settings] {
                    const auto ref = data->getReference();

                    if (ref == nullptr) {
                        vkh::logger::log_err("Please wait until the calculation is complete.");
                        return;
                    }

                    const uint64_t longestPeriod = ref->longestPeriod();

                    const std::unique_ptr<MB2Locator> locator = MB2Locator::locateMinibrot(
                            app.getState(), *data, *cache, getActionWhileFindingMBCenter(app, longestPeriod),
                            getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app),
                            getActionWhileFindingZoom(app));

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
    }

    std::function<void(uint64_t, int)> FnExplore::getActionWhileFindingMBCenter(RFF2 &app,
                                                                                const uint64_t longestPeriod) {
        return [&app, longestPeriod](const uint64_t p, int i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("Location : {:.3f}%[{}]",
                                                 static_cast<float>(100 * p) / static_cast<float>(longestPeriod), i));
            }
        };
    }

    std::function<void(uint64_t, float)> FnExplore::getActionWhileSeriesApprox(RFF2 &app) {
        return [&app](const uint64_t, const float i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("Series-Approximation : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(uint64_t, float)> FnExplore::getActionWhileCreatingTable(RFF2 &app) {
        return [&app](const uint64_t, const float i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("MP-Approximation : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(float)> FnExplore::getActionWhileFindingZoom(RFF2 &app) {
        return [&app](float zoom) {
            app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format("Zoom : 10^{}", zoom));
        };
    }
    std::function<void(uint64_t)> FnExplore::getActionWhileRefCalc(RFF2 &app, float startTime) {
        return [&app, startTime](const uint64_t p) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format(std::locale(), "Period : {:L}", p));
                app.setStatusMessage(Constants::Status::TIME_STATUS,
                                 std::format("Time : {}", Utilities::formatTime(time - startTime)));
            }
        };
    }
} // namespace merutilm::rff2
