//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFile.hpp"

#include "../constants/Constants.hpp"
#include "IOUtilities.h"
#include "SettingsMenu.hpp"
#include "../io/RFFLocationBinary.h"


namespace merutilm::rff2 {
    const std::function<void(SettingsMenu&, RenderScene&)> CallbackFile::SAVE_MAP = [](const SettingsMenu&, const RenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog(L"Save Map", Constants::Extension::DESC_DYNAMIC_MAP, IOUtilities::SAVE_FILE, Constants::Extension::DYNAMIC_MAP);
        if (path == nullptr) {
            return;
        }
        scene.generateMap().exportFile(*path);
    };
    const std::function<void(SettingsMenu&, RenderScene&)> CallbackFile::SAVE_IMAGE = [](const SettingsMenu&, RenderScene& scene) {
        scene.getRequests().requestCreateImage();
    };
    const std::function<void(SettingsMenu&, RenderScene&)> CallbackFile::SAVE_LOCATION = [](const SettingsMenu&, RenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog(L"Save Location", Constants::Extension::DESC_LOCATION, IOUtilities::SAVE_FILE, Constants::Extension::LOCATION);
        if (path == nullptr) {
            return;
        }
        const auto settings = scene.getAttribute().fractal; // clone the attr
        const auto &center = settings.center;
        RFFLocationBinary(settings.logZoom, center.real.to_string(), center.imag.to_string(), settings.maxIteration).exportFile(*path);
    };
    const std::function<void(SettingsMenu&, RenderScene&)> CallbackFile::LOAD_MAP = [](const SettingsMenu&, RenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog(L"Load Map", Constants::Extension::DESC_DYNAMIC_MAP, IOUtilities::OPEN_FILE, Constants::Extension::DYNAMIC_MAP);
        if (path == nullptr) {
            return;
        }
        scene.overwriteMatrixFromMap(RFFDynamicMapBinary::read(*path));
    };
    const std::function<void(SettingsMenu&, RenderScene&)> CallbackFile::LOAD_LOCATION = [](SettingsMenu&, RenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog(L"Load Map", Constants::Extension::DESC_LOCATION, IOUtilities::OPEN_FILE, Constants::Extension::LOCATION);
        if (path == nullptr) {
            return;
        }
        const RFFLocationBinary location = RFFLocationBinary::read(*path);

        scene.getAttribute().fractal.center = fp_complex(location.getReal(), location.getImag(), Perturbator::logZoomToExp10(location.getLogZoom()));
        scene.getAttribute().fractal.logZoom = location.getLogZoom();
        scene.getAttribute().fractal.maxIteration = location.getMaxIteration();
        scene.getRequests().requestRecompute();
    };
}
