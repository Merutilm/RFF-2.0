//
// Created by Merutilm on 2025-05-14.
//

#include "RFFCallbackRender.h"

#include "RFFSettingsMenu.h"


const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackRender::SET_RESOLUTION = [](RFFSettingsMenu&, RFFRenderScene&) {
};
const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RFFCallbackRender::ANTIALIASING = [](RFFSettingsMenu&, RFFRenderScene&) {
};
