//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackFile.h"

#include <iostream>

#include "IOUtilities.h"
#include "RFFSettingsMenu.h"


const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackFile::OPEN_MAP = [](RFFSettingsMenu&, RFFRenderScene& scene) {
    const auto path = IOUtilities::ioFileDialog("Open Map", "RFF Map file", IOUtilities::OPEN_FILE, {RFF::Extension::MAP});
    if (path == nullptr) {
        return;
    }
    scene.setCurrentMap(RFFMap::read(*path));
    scene.overwriteMatrixFromMap();
};
const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackFile::SAVE_MAP = [](RFFSettingsMenu&, RFFRenderScene&) {

};
const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackFile::SAVE_IMAGE = [](RFFSettingsMenu&, RFFRenderScene& scene) {
    scene.requestCreateImage();
};
