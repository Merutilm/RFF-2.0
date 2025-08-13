//
// Created by Merutilm on 2025-05-16.
//

#include "CallbackExplore.h"

#include <format>

#include "Utilities.h"
#include <cassert>

#include "../constants/Constants.hpp"
#include "../locator/MandelbrotLocator.h"

namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackExplore::RECOMPUTE = [
            ](const SettingsMenu &, GLRenderScene &scene) {
        scene.requestRecompute();
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackExplore::RESET = [
            ](const SettingsMenu &, GLRenderScene &scene) {
        scene.getSettings() = GLRenderScene::defaultSettings();
        scene.requestColor();
        scene.requestResize();
        scene.requestRecompute();
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackExplore::CANCEL_RENDER = [
            ](const SettingsMenu &, GLRenderScene &scene) {
        scene.getState().cancel();
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackExplore::FIND_CENTER = [
            ](const SettingsMenu &, GLRenderScene &scene) {
        const MandelbrotPerturbator *perturbator = scene.getCurrentPerturbator();
        if (perturbator == nullptr || perturbator->getReference() == Constants::NullPointer::PROCESS_TERMINATED_REFERENCE) {
            return;
        }

        if (const std::unique_ptr<fp_complex> c = MandelbrotLocator::findCenter(perturbator); c == nullptr) {
            MessageBox(nullptr, "No center found!", "Caution", MB_OK | MB_ICONWARNING);
        } else {
            scene.getSettings().calculationSettings.center = *c;
            scene.requestRecompute();
        }
    };
    const std::function<void(SettingsMenu &, GLRenderScene &)> CallbackExplore::LOCATE_MINIBROT = [
            ](const SettingsMenu &, GLRenderScene &scene) {
        Settings &settings = scene.getSettings();

        if (settings.calculationSettings.reuseReferenceMethod != ReuseReferenceMethod::DISABLED) {
            MessageBox(nullptr, "Do not reuse reference!", "Caution", MB_OK | MB_ICONWARNING);
            return;
        }

        scene.getState().cancel();
        const MandelbrotPerturbator *perturbator = scene.getCurrentPerturbator();
        assert(perturbator != nullptr);

        scene.getState().createThread(
            [&scene, logZoom = settings.calculationSettings.logZoom, perturbator, &settings](
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
                    vkh::logger::log("Locate Minibrot Cancelled.");
                    return;
                }
                const CalculationSettings &locatorCalc = locator->perturbator->getCalculationSettings();
                settings.calculationSettings.center = locatorCalc.center;
                settings.calculationSettings.logZoom = locatorCalc.logZoom - MandelbrotLocator::MINIBROT_LOG_ZOOM_OFFSET;
                scene.requestRecompute();
            }
        );
    };


    std::function<void(uint64_t, int)> CallbackExplore::getActionWhileFindingMinibrotCenter(
        const GLRenderScene &scene, const float logZoom,
        const uint64_t longestPeriod) {
        return [&scene, logZoom, longestPeriod](const uint64_t p, int i) {
            if (p % Utilities::getRefreshInterval(logZoom) == 0) {
                scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                       std::format("L : {:.3f}%[{}]",
                                                   static_cast<float>(100 * p) / static_cast<float>(
                                                       longestPeriod),
                                                   i));
            }
        };
    }

    std::function<void(uint64_t, float)> CallbackExplore::getActionWhileCreatingTable(
        const GLRenderScene &scene, const float logZoom) {
        return [&scene, logZoom](const uint64_t p, const float i) {
            if (p % Utilities::getRefreshInterval(logZoom) == 0) {
                scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                       std::format("A : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(float)> CallbackExplore::getActionWhileFindingZoom(const GLRenderScene &scene) {
        return [&scene](float zoom) {
            scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                   std::format("Z : 10^{}", zoom));
        };
    }
}