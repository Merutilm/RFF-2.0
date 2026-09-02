//
// Created by Merutilm on 2025-05-16.
//

#include "FnExplore.hpp"

#include <format>

#include <cassert>
#include "Utilities.h"

#include "../constants/Constants.hpp"
#include "../mb/MB2Locator.h"

namespace merutilm::rff2 {


    void FnExplore::recompute(RFF2 &app) {
        if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
            return app.getRequests().requestRecompute();
        }
    }
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
    void FnExplore::moveCursorToCenter(RFF2 &app) {
        if (ImGui::Checkbox("Auto Move Cursor To Center", &app.getSettings().explore.autoMoveCursorToCenter)) {
            if (app.getSettings().explore.autoMoveCursorToCenter) app.moveCursorToCenter();
        }
    }

    void FnExplore::reuseReference(RFF2 &app) {
        auto& frt = app.getSettings().fractal;
        ImGui::Checkbox("Reuse Reference", &frt.reference.reuse);
    }

    void FnExplore::moveToCenter(RFF2 &app) {
        const MB2RenderDataBase * renderData = app.getCurrentRenderData();
        auto& frt = app.getSettings().fractal;
        if (renderData && renderData->getPerturbator()) {
            if (ImGui::Button("Move To Center", ImVec2(-FLT_MIN, 0))) {
                const int exp10 = Perturbator::logZoomToExp10(renderData->getReference()->logZoom);
                const auto off = MB2Locator::findCenterOffset(*renderData)->create_variant(exp10);
                fixed_point_complex_i1 center = frt.reference.center.create_variant(exp10);
                fixed_point_complex::add(center, center, off);
                frt.reference.center = center;
                app.getRequests().requestRecompute();
            }
        }
    }


    void FnExplore::goToOriginalReference(RFF2 &app) {

        MB2RenderDataBase * renderData = app.getCurrentRenderData();

        auto& frt = app.getSettings().fractal;
        if (frt.reference.reuse && renderData && renderData->getReference()) {
            if (ImGui::Button("Go to Original Reference", ImVec2(-FLT_MIN, 0))) {
                const float startTime = app.rootWindowContext->getWindow()->getTime();
                frt.reference.center = renderData->getReference()->center;
                frt.general.logZoom = renderData->getReference()->logZoom;
                renderData->translate(frt.general.logZoom, renderData->getReference()->dcMax, app.getSettings().fractal.perturb, frt.reference.center, getActionWhileSeriesApprox(app, startTime));
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
                    const uint64_t period = data->getReference()->longestPeriod();
                    const auto center = MB2Locator::locateMinibrot(
                            state, *data, *app.getApproxTableCache(), getActionWhileFindingMBCenter(app, period, startTime),
                            getActionWhileSeriesApprox(app, startTime), getActionWhileCreatingTable(app, startTime),
                            getActionWhileFindingZoom(app, startTime));
                    if (center == nullptr)
                        return;

                    FractalSettings refCalc = settings.fractal;
                    refCalc.reference.center = center->data->fractalSettings.reference.center;
                    refCalc.general.logZoom = center->data->fractalSettings.general.logZoom - MB2Locator::MINIBROT_LOG_ZOOM_OFFSET;
                    int refExp10 = Perturbator::logZoomToExp10(refCalc.general.logZoom);
                    if (refCalc.general.logZoom > Constants::Fractal::MULTITHREAD_ZOOM_THRESHOLD) {
                        data = std::make_unique<DexMB2RenderData>(
                                state, refCalc, *app.getApproxTableCache(), center->data->getPerturbator()->dcMax, refExp10,
                                data->getReference()->length(), 0, getActionWhileRefCalc(app, startTime), getActionWhileSeriesApprox(app, startTime), getActionWhileCreatingTable(app, startTime));
                    } else {
                        data = std::make_unique<DoubleMB2RenderData>(
                                state, refCalc, *app.getApproxTableCache(), center->data->getPerturbator()->dcMax, refExp10,
                                data->getReference()->length(), 0, getActionWhileRefCalc(app, startTime), getActionWhileSeriesApprox(app, startTime), getActionWhileCreatingTable(app, startTime));
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
                    const float startTime = app.rootWindowContext->getWindow()->getTime();

                    const std::unique_ptr<MB2Locator> locator = MB2Locator::locateMinibrot(
                            app.getState(), *data, *cache, getActionWhileFindingMBCenter(app, longestPeriod, startTime),
                            getActionWhileSeriesApprox(app, startTime), getActionWhileCreatingTable(app, startTime),
                            getActionWhileFindingZoom(app, startTime));

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
                                                                                const uint64_t longestPeriod, const float startTime) {
        return [&app, longestPeriod, startTime](const uint64_t p, int i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("Location : {:.3f}%[{}]",
                                     static_cast<float>(100 * p) / static_cast<float>(longestPeriod), i));
                app.setStatusMessage(Constants::Status::TIME_STATUS,
                                 std::format("Time : {}", Utilities::formatTime(time - startTime)));
            }
        };
    }

    std::function<void(uint64_t, float)> FnExplore::getActionWhileSeriesApprox(RFF2 &app, const float startTime) {
        return [&app, startTime](const uint64_t it, const float i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                std::format("Series-Approximation : {:.3f}%", i * 100, it));
                app.setStatusMessage(Constants::Status::TIME_STATUS,
                                 std::format("Time : {}", Utilities::formatTime(time - startTime)));
            }
        };
    }


    std::function<void(uint64_t, float)> FnExplore::getActionWhileCreatingTable(RFF2 &app, const float startTime) {
        return [&app, startTime](const uint64_t, const float i) {

            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("MP-Approximation : {:.3f}%", i * 100));

                app.setStatusMessage(Constants::Status::TIME_STATUS,
                                 std::format("Time : {}", Utilities::formatTime(time - startTime)));
            }
        };
    }


    std::function<void(float)> FnExplore::getActionWhileFindingZoom(RFF2 &app, const float startTime) {
        return [&app, startTime](float zoom) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format("Zoom : 10^{}", zoom));
                app.setStatusMessage(Constants::Status::TIME_STATUS,
                                 std::format("Time : {}", Utilities::formatTime(time - startTime)));
            }
        };
    }
    std::function<void(uint64_t)> FnExplore::getActionWhileRefCalc(RFF2 &app, const float startTime) {
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
