//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RFFRenderScene.h"
#include "RFFSettingsMenu.h"

struct RFFCallbackFile {
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> OPEN_MAP;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> SAVE_MAP;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> SAVE_IMAGE;
};
