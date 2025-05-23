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
    const MandelbrotPerturbator* perturbator = scene.getCurrentPerturbator();
    assert(perturbator != nullptr);

    scene.getState().createThread(
        [&scene, logZoom = settings.calculationSettings.logZoom, perturbator, &settings](
    std::stop_token)  {

            ApproxTableCache &approxTableCache = scene.getApproxTableCache();
            int interval = RFFUtilities::getRefreshInterval(logZoom);
            uint64_t longestPeriod = perturbator->getReference()->longestPeriod();

            const std::unique_ptr<MandelbrotLocator> locator = MandelbrotLocator::locateMinibrot(
                scene.getState(), perturbator, approxTableCache,

                [interval, &scene, longestPeriod](const uint64_t p, int i) {
                    if (p % interval == 0) {
                        scene.setStatusMessage(RFF::Status::RENDER_STATUS,
                                               std::format("C : {:.3f}%[{}]",
                                                           static_cast<float>(100 * p) / static_cast<float>(
                                                               longestPeriod),
                                                           i));
                    }
                },
                [interval, &scene](const uint64_t p, const float i) {
                    if (p % interval == 0) {
                        scene.setStatusMessage(RFF::Status::RENDER_STATUS,
                                               std::format("MPA : {:.3f}%", i * 100));
                    }
                },
                [ &scene](float zoom) {
                    scene.setStatusMessage(RFF::Status::RENDER_STATUS,
                                           std::format("Z : 10^{}", zoom));
                }
            );

            if (locator == nullptr) {
                RFFUtilities::log("Locate Minibrot Cancelled.");
                return;
            }
            std::unique_ptr<MandelbrotPerturbator> locatorPerturbator = std::move(locator->perturbator);
            const CalculationSettings &locatorCalc = locatorPerturbator->getCalculationSettings();
            settings.calculationSettings.center = locatorCalc.center;
            settings.calculationSettings.logZoom = locatorCalc.logZoom - MandelbrotLocator::MINIBROT_LOG_ZOOM_OFFSET;
            scene.setCurrentPerturbator(std::move(locatorPerturbator));
            scene.requestRecompute();
        }
    );
};
