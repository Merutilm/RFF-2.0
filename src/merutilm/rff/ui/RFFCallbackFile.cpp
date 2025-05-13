//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackFile.h"

#include <iostream>

#include "RFFSettingsMenu.h"


const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackFile::OPEN_MAP = [](RFFSettingsMenu&, RFFRenderScene&) {
    std::cout << "RFFCallbackFile::OPEN_MAP" << std::endl;
};
const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackFile::SAVE_MAP = [](RFFSettingsMenu&, RFFRenderScene&) {

};
const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackFile::SAVE_IMAGE = [](RFFSettingsMenu&, RFFRenderScene&) {

};
