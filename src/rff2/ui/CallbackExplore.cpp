//
// Created by Merutilm on 2025-05-16.
//

#include "CallbackExplore.hpp"

#include <format>

#include "Utilities.h"
#include <cassert>

#include "RenderScene.hpp"
#include "../constants/Constants.hpp"
#include "../locator/MandelbrotLocator.h"

namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackExplore::RECOMPUTE = [
            ](const SettingsMenu &, RenderScene &scene) {
        scene.getRequests().requestRecompute();
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackExplore::RESET = [
            ](const SettingsMenu &, RenderScene &scene) {
        scene.getRequests().requestDefaultSettings();
        scene.getRequests().requestShader();
        scene.getRequests().requestResize();
        scene.getRequests().requestRecompute();
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackExplore::CANCEL_RENDER = [
            ](const SettingsMenu &, RenderScene &scene) {
        scene.getState().cancel();
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackExplore::FIND_CENTER = [
            ](const SettingsMenu &, RenderScene &scene) {
        const MandelbrotPerturbator *perturbator = scene.getCurrentPerturbator();
        if (perturbator == nullptr || perturbator->getReference() == Constants::NullPointer::PROCESS_TERMINATED_REFERENCE) {
            return;
        }

        if (const std::unique_ptr<fp_complex> c = MandelbrotLocator::findCenter(perturbator); c == nullptr) {
            MessageBox(nullptr, "No center found!", "Caution", MB_OK | MB_ICONWARNING);
        } else {
            scene.getAttribute().fractal.center = *c;
            scene.getRequests().requestRecompute();
        }
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackExplore::LOCATE_MINIBROT = [
            ](const SettingsMenu &, RenderScene &scene) {
        Attribute &settings = scene.getAttribute();

        if (settings.fractal.reuseReferenceMethod != FrtReuseReferenceMethod::DISABLED) {
            MessageBox(nullptr, "Do not reuse reference!", "Caution", MB_OK | MB_ICONWARNING);
            return;
        }

        scene.getState().cancel();
        const MandelbrotPerturbator *perturbator = scene.getCurrentPerturbator();
        if(perturbator == nullptr) {
            throw vkh::exception_invalid_state("Perturbator cannot be null");
        }

        scene.getState().createThread(
            [&scene, logZoom = settings.fractal.logZoom, perturbator, &settings](
        const std::stop_token&) {
                ApproxTableCache &approxTableCache = scene.getApproxTableCache();
                const uint64_t longestPeriod = perturbator->getReference()->longestPeriod();

                const std::unique_ptr<MandelbrotLocator> locator = MandelbrotLocator::locateMinibrot(
                    scene.getState(), perturbator, approxTableCache,
                    getActionWhileFindingMinibrotCenter(scene, logZoom, longestPeriod),
                    getActionWhileCreatingTable(scene, logZoom),
                    getActionWhileFindingZoom(scene)
                );

                if (locator == nullptr) {
                    vkh::logger::w_log(L"Locate Minibrot Cancelled.");
                    return;
                }
                const FractalAttribute &locatorCalc = locator->perturbator->getCalculationSettings();
                settings.fractal.center = locatorCalc.center;
                settings.fractal.logZoom = locatorCalc.logZoom - MandelbrotLocator::MINIBROT_LOG_ZOOM_OFFSET;
                scene.getRequests().requestRecompute();
            }
        );
    };


    std::function<void(uint64_t, int)> CallbackExplore::getActionWhileFindingMinibrotCenter(
        const RenderScene &scene, const float logZoom,
        const uint64_t longestPeriod) {
        return [&scene, logZoom, longestPeriod](const uint64_t p, int i) {
            if (p % Utilities::getRefreshInterval(logZoom) == 0) {
                scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                       std::format(L"L : {:.3f}%[{}]",
                                                   static_cast<float>(100 * p) / static_cast<float>(
                                                       longestPeriod),
                                                   i));
            }
        };
    }

    std::function<void(uint64_t, float)> CallbackExplore::getActionWhileCreatingTable(
        const RenderScene &scene, const float logZoom) {
        return [&scene, logZoom](const uint64_t p, const float i) {
            if (p % Utilities::getRefreshInterval(logZoom) == 0) {
                scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                       std::format(L"A : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(float)> CallbackExplore::getActionWhileFindingZoom(const RenderScene &scene) {
        return [&scene](float zoom) {
            scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                   std::format(L"Z : 10^{}", zoom));
        };
    }
}