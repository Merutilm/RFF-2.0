//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <functional>

#include "RFFRenderScene.h"
#include "RFFSettingsMenu.h"

struct RFFCallbackVideo {
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> DATA_SETTINGS;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> ANIMATION_SETTINGS;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> EXPORT_SETTINGS;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> GENERATE_VID_KEYFRAME;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> EXPORT_ZOOM_VID;

};
