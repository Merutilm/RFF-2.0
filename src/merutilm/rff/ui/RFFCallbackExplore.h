//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <functional>

#include "RFFSettingsMenu.h"

struct RFFCallbackExplore {

    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RECOMPUTE;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> REFRESH_COLOR;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> RESET;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> CANCEL_RENDER;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> FIND_CENTER;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> LOCATE_MINIBROT;
};
