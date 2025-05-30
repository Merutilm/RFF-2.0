//
// Created by Merutilm on 2025-05-16.
//

#include "RFFCallbackExplore.h"

#include <format>

#include "RFFUtilities.h"
#include <assert.h>
#include "../locator/MandelbrotLocator.h"

const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackExplore::RECOMPUTE = [
        ](const RFFSettingsMenu &, RFFRenderScene &scene) {
    scene.requestRecompute();
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackExplore::REFRESH_COLOR = [
        ](const RFFSettingsMenu &, RFFRenderScene &scene) {
    scene.requestColor();
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackExplore::RESET = [
        ](const RFFSettingsMenu &, RFFRenderScene &scene) {
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackExplore::CANCEL_RENDER = [
        ](const RFFSettingsMenu &, RFFRenderScene &scene) {
    scene.getState().cancel();
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackExplore::FIND_CENTER = [
        ](const RFFSettingsMenu &, RFFRenderScene &scene) {
    const MandelbrotPerturbator *perturbator = scene.getCurrentPerturbator();
    if (perturbator == nullptr || perturbator->getReference() == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE) {
        return;
    }

    if (const std::unique_ptr<fp_complex> c = MandelbrotLocator::findCenter(perturbator); c == nullptr) {
        MessageBox(nullptr, "No center found!", "Caution", MB_OK | MB_ICONWARNING);
    } else {
        scene.getSettings().calculationSettings.center = *c;
        scene.requestRecompute();
    }
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackExplore::LOCATE_MINIBROT = [
        ](const RFFSettingsMenu &, RFFRenderScene &scene) {
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
                RFFUtilities::log("Locate Minibrot Cancelled.");
                return;
            }
            const CalculationSettings &locatorCalc = locator->perturbator->getCalculationSettings();
            settings.calculationSettings.center = locatorCalc.center;
            settings.calculationSettings.logZoom = locatorCalc.logZoom - MandelbrotLocator::MINIBROT_LOG_ZOOM_OFFSET;
            scene.requestRecompute();
        }
    );
};


std::function<void(uint64_t, int)> RFFCallbackExplore::getActionWhileFindingMinibrotCenter(
    const RFFRenderScene &scene, const float logZoom,
    const uint64_t longestPeriod) {
    return [&scene, logZoom, longestPeriod](const uint64_t p, int i) {
        if (p % RFFUtilities::getRefreshInterval(logZoom) == 0) {
            scene.setStatusMessage(RFF::Status::RENDER_STATUS,
                                   std::format("C : {:.3f}%[{}]",
                                               static_cast<float>(100 * p) / static_cast<float>(
                                                   longestPeriod),
                                               i));
        }
    };
}

std::function<void(uint64_t, float)> RFFCallbackExplore::getActionWhileCreatingTable(
    const RFFRenderScene &scene, const float logZoom) {
    return [&scene, logZoom](const uint64_t p, const float i) {
        if (p % RFFUtilities::getRefreshInterval(logZoom) == 0) {
            scene.setStatusMessage(RFF::Status::RENDER_STATUS,
                                   std::format("A : {:.3f}%", i * 100));
        }
    };
}


std::function<void(float)> RFFCallbackExplore::getActionWhileFindingZoom(const RFFRenderScene &scene) {
    return [&scene](float zoom) {
        scene.setStatusMessage(RFF::Status::RENDER_STATUS,
                               std::format("Z : 10^{}", zoom));
    };
}
