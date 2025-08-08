//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFile.h"

#include "../constants/Constants.hpp"
#include "IOUtilities.h"
#include "SettingsMenu.h"
#include "../io/RFFLocationBinary.h"


namespace merutilm::rff2 {
    const std::function<void(SettingsMenu&, GLRenderScene&)> CallbackFile::SAVE_MAP = [](const SettingsMenu&, const GLRenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog("Save Map", Constants::Extension::DESC_DYNAMIC_MAP, IOUtilities::SAVE_FILE, Constants::Extension::DYNAMIC_MAP);
        if (path == nullptr) {
            return;
        }
        scene.getCurrentMap().exportFile(*path);
    };
    const std::function<void(SettingsMenu&, GLRenderScene&)> CallbackFile::SAVE_IMAGE = [](const SettingsMenu&, GLRenderScene& scene) {
        scene.requestCreateImage();
    };
    const std::function<void(SettingsMenu&, GLRenderScene&)> CallbackFile::SAVE_LOCATION = [](const SettingsMenu&, GLRenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog("Save Location", Constants::Extension::DESC_LOCATION, IOUtilities::SAVE_FILE, Constants::Extension::LOCATION);
        if (path == nullptr) {
            return;
        }
        const auto settings = scene.getSettings().calculationSettings; // clone the settings
        const auto &center = settings.center;
        RFFLocationBinary(settings.logZoom, center.real.to_string(), center.imag.to_string(), settings.maxIteration).exportFile(*path);
    };
    const std::function<void(SettingsMenu&, GLRenderScene&)> CallbackFile::LOAD_MAP = [](const SettingsMenu&, GLRenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog("Load Map", Constants::Extension::DESC_DYNAMIC_MAP, IOUtilities::OPEN_FILE, Constants::Extension::DYNAMIC_MAP);
        if (path == nullptr) {
            return;
        }
        scene.setCurrentMap(RFFDynamicMapBinary::read(*path));
        scene.overwriteMatrixFromMap();
    };
    const std::function<void(SettingsMenu&, GLRenderScene&)> CallbackFile::LOAD_LOCATION = [](SettingsMenu&, GLRenderScene& scene) {
        const auto path = IOUtilities::ioFileDialog("Load Map", Constants::Extension::DESC_LOCATION, IOUtilities::OPEN_FILE, Constants::Extension::LOCATION);
        if (path == nullptr) {
            return;
        }
        const RFFLocationBinary location = RFFLocationBinary::read(*path);

        scene.getSettings().calculationSettings.center = fp_complex(location.getReal(), location.getImag(), Perturbator::logZoomToExp10(location.getLogZoom()));
        scene.getSettings().calculationSettings.logZoom = location.getLogZoom();
        scene.getSettings().calculationSettings.maxIteration = location.getMaxIteration();
        scene.requestRecompute();
    };
}
