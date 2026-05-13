//
// Created by Merutilm on 2025-05-16.
//

#include "CallbackExplore.hpp"

#include <format>

#include <cassert>
#include "Utilities.h"

#include "../constants/Constants.hpp"
#include "../locator/MB2Locator.h"
#include "AppRenderManager.hpp"

namespace merutilm::rff2 {


    std::function<void()> CallbackExplore::fnRecompute(AppRenderManager &arm) {
        return [&arm] { arm.getRequests().requestRecompute(); };
    };
    std::function<void()> CallbackExplore::fnReset(AppRenderManager &arm) {
        return [&arm] {
            arm.getRequests().requestDefaultSettings();
            arm.getRequests().requestShader();
            arm.getRequests().requestResize();
            arm.getRequests().requestRecompute();
        };
    }
    std::function<void()> CallbackExplore::fnCancelRender(AppRenderManager &arm) {
        return [&arm] { arm.getState().cancel(); };
    }
    std::function<void()> CallbackExplore::fnFindCenter(AppRenderManager &arm) {
        return [&arm] {
            const MB2RenderDataBase *data = arm.getCurrentRenderData();
            if (data == nullptr || !data->getReference())
                return;

            if (const std::unique_ptr<fixed_point_complex_i1> c = MB2Locator::findCenter(*data); c == nullptr) {
                MessageBox(nullptr, "No center found!", "Caution", MB_OK | MB_ICONWARNING);
            } else {
                arm.getSettings().fractal.reference.center = *c;
                arm.getRequests().requestRecompute();
            }
        };
    }
    std::function<void()> CallbackExplore::fnLocateMinibrot(AppRenderManager &arm) {
        return [&arm] {
            Settings &settings = arm.getSettings();

            if (settings.fractal.reference.reuse != FrtReferenceReuseMethod::DISABLED) {
                MessageBox(nullptr, "Do not reuse reference!", "Caution", MB_OK | MB_ICONWARNING);
                return;
            }

            arm.getState().cancel();
            const MB2RenderDataBase *data = arm.getCurrentRenderData();
            if (data == nullptr) {
                throw vkh::exception_invalid_state("Perturbator cannot be null");
            }

            arm.getState().createThread(
                    [&arm, logZoom = settings.fractal.general.logZoom, data, &settings](const std::stop_token &) {
                        const auto ref = data->getReference();

                        if (ref == nullptr) {
                            MessageBox(nullptr, "Please wait until the calculation is complete.", "Caution",
                                       MB_OK | MB_ICONWARNING);
                            return;
                        }

                        const uint64_t longestPeriod = ref->longestPeriod();

                        const std::unique_ptr<MB2Locator> locator = MB2Locator::locateMinibrot(
                                arm.getState(), data, getActionWhileFindingMBCenter(arm, logZoom, longestPeriod),
                                getActionWhileCreatingTable(arm, logZoom), getActionWhileFindingZoom(arm));

                        if (locator == nullptr) {
                            vkh::logger::w_log(L"Locate Minibrot Cancelled.");
                            return;
                        }
                        const FractalSettings &locatorCalc = locator->data->fractalSettings;
                        settings.fractal.reference.center = locatorCalc.reference.center;
                        settings.fractal.general.logZoom = locatorCalc.general.logZoom - MB2Locator::MINIBROT_LOG_ZOOM_OFFSET;
                        arm.getRequests().requestRecompute();
                    });
        };
    }

    std::function<void(uint64_t, int)>
    CallbackExplore::getActionWhileFindingMBCenter(const AppRenderManager &scene, const float logZoom,
                                                         const uint64_t longestPeriod) {
        return [&scene, logZoom, longestPeriod](const uint64_t p, int i) {
            if (p % Utilities::getRefreshInterval(logZoom) == 0) {
                scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                       std::format(L"L : {:.3f}%[{}]",
                                                   static_cast<float>(100 * p) / static_cast<float>(longestPeriod), i));
            }
        };
    }

    std::function<void(uint64_t, float)> CallbackExplore::getActionWhileCreatingTable(const AppRenderManager &scene,
                                                                                      const float logZoom) {
        return [&scene, logZoom](const uint64_t p, const float i) {
            if (p % Utilities::getRefreshInterval(logZoom) == 0) {
                scene.setStatusMessage(Constants::Status::RENDER_STATUS, std::format(L"A : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(float)> CallbackExplore::getActionWhileFindingZoom(const AppRenderManager &scene) {
        return [&scene](float zoom) {
            scene.setStatusMessage(Constants::Status::RENDER_STATUS, std::format(L"Z : 10^{}", zoom));
        };
    }
} // namespace merutilm::rff2
