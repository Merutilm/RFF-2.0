//
// Created by Merutilm on 2025-05-16.
//

#include "FnExplore.hpp"

#include <format>

#include <cassert>
#include "../util/Utilities.h"

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
            if (app.getSettings().explore.autoMoveCursorToCenter)
                app.moveCursorToCenter();
        }
    }

    void FnExplore::reuseReference(RFF2 &app) {
        auto &frt = app.getSettings().fractal;
        ImGui::Checkbox("Reuse Reference", &frt.reference.reuse);
    }

    void FnExplore::moveToCenter(RFF2 &app) {
        const MB2RenderDataBase *renderData = app.getCurrentRenderData();
        auto &frt = app.getSettings().fractal;
        if (renderData && renderData->getPerturbator()) {
            if (ImGui::Button("Move To Center", ImVec2(-FLT_MIN, 0))) {
                const int exp10 = Perturbator::logZoomToExp10(renderData->getReference()->logZoom);
                const auto off = MB2Locator::findCenterOffset(*renderData->getReference())->create_variant(exp10);
                fixed_point_complex center = frt.reference.center.create_variant(exp10);
                fixed_point_complex::add(center, center, off);
                frt.reference.center = center;
                app.getRequests().requestRecompute();
            }
        }
    }


    void FnExplore::goToOriginalReference(RFF2 &app) {

        MB2RenderDataBase *renderData = app.getCurrentRenderData();

        auto &frt = app.getSettings().fractal;
        if (frt.reference.reuse && renderData && renderData->getReference()) {
            if (ImGui::Button("Go to Original Reference", ImVec2(-FLT_MIN, 0))) {
                const float startTime = app.rootWindowContext->getWindow()->getTime();
                frt.reference.center = renderData->getReference()->center;
                frt.general.logZoom = renderData->getReference()->logZoom;
                renderData->translate(frt.general.logZoom, renderData->getReference()->dcMax,
                                      app.getSettings().fractal.perturb, frt.reference.center,
                                      app.getActionWhileSeriesApprox(startTime));
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
                            state, *data, app.getActionWhileFindingMBCenter(period, startTime));
                    if (center == std::nullopt)
                        return;

                    FractalSettings frt = settings.fractal;
                    frt.reference.center = center->center;
                    frt.general.logZoom = center->logZoom;
                    const dex dcMax = app.getDcMax(frt.general.logZoom, settings.render.display.clarityMultiplier);
                    const int refExp10 = Perturbator::logZoomToExp10(frt.general.logZoom);
                    data = app.createAppropriateRenderData(settings.render.computeShader.use, frt.general.logZoom,
                                                           startTime, frt, dcMax,
                                                           refExp10, data->getReference()->length(), data->getReference()->longestPeriod(), 0);

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

                    const auto locator = MB2Locator::locateMinibrot(
                            app.getState(), *data, app.getActionWhileFindingMBCenter(longestPeriod, startTime));

                    if (locator == std::nullopt) {
                        vkh::logger::log("Locate Minibrot Cancelled.");
                        return;
                    }
;
                    settings.fractal.reference.center = locator->center;
                    settings.fractal.general.logZoom = locator->logZoom;
                    app.getRequests().requestRecompute();
                });
            }
        }
    }

} // namespace merutilm::rff2
