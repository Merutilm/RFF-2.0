//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include "RFFRenderScene.h"
#include "RFFSettingsMenu.h"

struct RFFCallbackRender {
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> SET_RESOLUTION;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> ANTIALIASING;

};
