//
// Created by Merutilm on 2025-05-16.
//

#include "CallbackExplore.h"

#include <format>

#include "Utilities.h"
#include <assert.h>

#include "Constants.h"
#include "../locator/MandelbrotLocator.h"

const std::function<void(merutilm::rff::SettingsMenu &, merutilm::rff::RenderScene &)> merutilm::rff::CallbackExplore::RECOMPUTE = [
        ](const SettingsMenu &, RenderScene &scene) {
    scene.requestRecompute();
};
const std::function<void(merutilm::rff::SettingsMenu &, merutilm::rff::RenderScene &)> merutilm::rff::CallbackExplore::REFRESH_COLOR = [
        ](const SettingsMenu &, RenderScene &scene) {
    scene.requestColor();
};
const std::function<void(merutilm::rff::SettingsMenu &, merutilm::rff::RenderScene &)> merutilm::rff::CallbackExplore::RESET = [
        ](const SettingsMenu &, RenderScene &scene) {
};
const std::function<void(merutilm::rff::SettingsMenu &, merutilm::rff::RenderScene &)> merutilm::rff::CallbackExplore::CANCEL_RENDER = [
        ](const SettingsMenu &, RenderScene &scene) {
    scene.getState().cancel();
};
const std::function<void(merutilm::rff::SettingsMenu &, merutilm::rff::RenderScene &)> merutilm::rff::CallbackExplore::FIND_CENTER = [
        ](const SettingsMenu &, RenderScene &scene) {
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
const std::function<void(merutilm::rff::SettingsMenu &, merutilm::rff::RenderScene &)> merutilm::rff::CallbackExplore::LOCATE_MINIBROT = [
        ](const SettingsMenu &, RenderScene &scene) {
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
    std::stop_token) {
            ApproxTableCache &approxTableCache = scene.getApproxTableCache();
            const uint64_t longestPeriod = perturbator->getReference()->longestPeriod();

            const std::unique_ptr<MandelbrotLocator> locator = MandelbrotLocator::locateMinibrot(
                scene.getState(), perturbator, approxTableCache,
                getActionWhileFindingMinibrotCenter(scene, logZoom, longestPeriod),
                getActionWhileCreatingTable(scene, logZoom),
                getActionWhileFindingZoom(scene)
            );

            if (locator == nullptr) {
                Utilities::log("Locate Minibrot Cancelled.");
                return;
            }
            const CalculationSettings &locatorCalc = locator->perturbator->getCalculationSettings();
            settings.calculationSettings.center = locatorCalc.center;
            settings.calculationSettings.logZoom = locatorCalc.logZoom - MandelbrotLocator::MINIBROT_LOG_ZOOM_OFFSET;
            scene.requestRecompute();
        }
    );
};


std::function<void(uint64_t, int)> merutilm::rff::CallbackExplore::getActionWhileFindingMinibrotCenter(
    const RenderScene &scene, const float logZoom,
    const uint64_t longestPeriod) {
    return [&scene, logZoom, longestPeriod](const uint64_t p, int i) {
        if (p % Utilities::getRefreshInterval(logZoom) == 0) {
            scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                   std::format("C : {:.3f}%[{}]",
                                               static_cast<float>(100 * p) / static_cast<float>(
                                                   longestPeriod),
                                               i));
        }
    };
}

std::function<void(uint64_t, float)> merutilm::rff::CallbackExplore::getActionWhileCreatingTable(
    const RenderScene &scene, const float logZoom) {
    return [&scene, logZoom](const uint64_t p, const float i) {
        if (p % Utilities::getRefreshInterval(logZoom) == 0) {
            scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                                   std::format("A : {:.3f}%", i * 100));
        }
    };
}


std::function<void(float)> merutilm::rff::CallbackExplore::getActionWhileFindingZoom(const RenderScene &scene) {
    return [&scene](float zoom) {
        scene.setStatusMessage(Constants::Status::RENDER_STATUS,
                               std::format("Z : 10^{}", zoom));
    };
}
