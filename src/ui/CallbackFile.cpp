//
// Created by Merutilm on 2025-05-14.
//

#include "CallbackFile.h"

#include <iostream>

#include "Constants.h"
#include "IOUtilities.h"
#include "SettingsMenu.h"


const std::function<void(merutilm::rff::SettingsMenu&, merutilm::rff::RenderScene&)> merutilm::rff::CallbackFile::OPEN_MAP = [](SettingsMenu&, RenderScene& scene) {
    const auto path = IOUtilities::ioFileDialog("Open Map", "RFF Map file", IOUtilities::OPEN_FILE, {Constants::Extension::MAP});
    if (path == nullptr) {
        return;
    }
    scene.setCurrentMap(RFFMap::read(*path));
    scene.overwriteMatrixFromMap();
};
const std::function<void(merutilm::rff::SettingsMenu&, merutilm::rff::RenderScene&)> merutilm::rff::CallbackFile::SAVE_MAP = [](SettingsMenu&, RenderScene&) {

};
const std::function<void(merutilm::rff::SettingsMenu&, merutilm::rff::RenderScene&)> merutilm::rff::CallbackFile::SAVE_IMAGE = [](SettingsMenu&, RenderScene& scene) {
    scene.requestCreateImage();
};
