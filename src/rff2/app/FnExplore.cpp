//
// Created by Merutilm on 2025-05-16.
//

#include "FnExplore.hpp"

#include <format>

#include <cassert>
#include "../util/Utilities.h"
#include "../util/ImGuiUtils.hpp"

#include "../mb/MB2Locator.hpp"

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
        if (ImGui::Checkbox("Auto Move Cursor To Center", &app.getSettings().explore.autoMoveCursorToCenter) &&
            app.getSettings().explore.autoMoveCursorToCenter) {
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
        const bool disable = !renderData || !renderData->getReference();
        if (disable) ImGui::BeginDisabled();
        if (!frt.reference.reuse && ImGui::Button("Move To Center", ImVec2(-FLT_MIN, 0))) {
            assert(renderData->getReference());
            const int64_t exp10 = Perturbator::getExp10WithSettings(frt.reference, renderData->getReference()->logZoom);
            const fixed_point_complex off = MB2Locator::calcCenterOffset(*renderData->getReference());
            fixed_point_complex center = frt.reference.center.create_variant(exp10);
            fixed_point_complex::add(center, center, off);
            frt.reference.center = std::move(center);
            app.getRequests().requestRecompute();
        }
        if (disable) ImGui::EndDisabled();
    }


    void FnExplore::goToOriginalReference(RFF2 &app) {

        MB2RenderDataBase *renderData = app.getCurrentRenderData();

        auto &frt = app.getSettings().fractal;

        const bool disable = !renderData || !renderData->getReference();
        if (disable) ImGui::BeginDisabled();
        if (frt.reference.reuse &&
            ImGui::Button("Go to Original Reference", ImVec2(-FLT_MIN, 0))) {
            assert(renderData->getReference());
            const double startTime = app.rootWindowContext->getWindow()->getTime();
            frt.reference.center = renderData->getReference()->center;
            frt.general.logZoom = renderData->getReference()->logZoom;
            renderData->translate(frt.general.logZoom, renderData->getReference()->dcMax,
                                  app.getSettings().fractal.perturb, frt.reference.center,
                                  app.getFnSeriesApprox(startTime));
            app.getRequests().requestRecompute();
        }
        if (disable) ImGui::EndDisabled();
    }

    void FnExplore::locateCenteredReference(RFF2 &app) {

        std::unique_ptr<MB2RenderDataBase> &data = app.getCurrentRenderDataOwnRef();
        Settings &settings = app.getSettings();

        const bool disable = !data || !data->getReference();
        if (disable) ImGui::BeginDisabled();

        if (!settings.fractal.reference.reuse &&
            ImGui::Button("Locate Centered Reference", ImVec2(-FLT_MIN, 0))) {
            ParallelRenderState &state = app.getState();

            state.createThread([&app, &data, &settings] {
                const double startTime = app.rootWindowContext->getWindow()->getTime();

                MB2Locator locator(app.getState(), *data, settings.explore.locator,
                                   app.getFnFindingMBCenter(startTime));
                const std::optional<MB2LocateResult> result = locator.locate();
                if (result == std::nullopt)
                    return;

                FractalSettings frt = settings.fractal;
                frt.reference.center = result->center;
                frt.general.logZoom = result->logZoom;
                const dex dcMax = app.calcDcMax(frt.general.logZoom, settings.render.display.clarityMultiplier);
                const int64_t refExp10 = Perturbator::getExp10WithSettings(frt.reference, frt.general.logZoom);
                data = app.createAppropriateRenderData(settings.render.computeShader.use, frt.general.logZoom,
                                                       startTime, frt, dcMax, refExp10, data->getReference()->length());

                settings.fractal.reference.reuse = true;
                app.getRequests().requestRecompute();
            });
        }
        if (disable) ImGui::EndDisabled();

    }

    void FnExplore::locateMinibrot(RFF2 &app) {

        Settings &settings = app.getSettings();
        const MB2RenderDataBase *data = app.getCurrentRenderData();
        const bool disabled = !data || !data->getReference();
        if (disabled) ImGui::BeginDisabled();

        if (!settings.fractal.reference.reuse) {
            ImGui::Separator();

            ImGui::Checkbox("Burst-locate", &settings.explore.locator.burst);
            ImGuiUtils::HelpMarker("It significantly increases locate speed at the expense of stability.");

            if (ImGui::Button("Locate Minibrot", ImVec2(-FLT_MIN, 0))) {

                app.getState().cancel();
                app.getState().createThread([&app, data, &settings] {
                    const auto ref = data->getReference();

                    if (ref == nullptr) {
                        vkh::logger::log_err("Please wait until the calculation is complete.");
                        return;
                    }

                    const double startTime = app.rootWindowContext->getWindow()->getTime();

                    MB2Locator locator(app.getState(), *data, settings.explore.locator,
                                       app.getFnFindingMBCenter(startTime));
                    const std::optional<MB2LocateResult> result = locator.locate();

                    if (result == std::nullopt) {
                        vkh::logger::log("Locate Minibrot Cancelled.");
                        return;
                    }

                    settings.fractal.reference.center = result->center;
                    settings.fractal.general.logZoom = result->logZoom;
                    app.getRequests().requestRecompute();
                });
            }
        }
        if (disabled) ImGui::EndDisabled();
    }

} // namespace merutilm::rff2
