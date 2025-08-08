//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <functional>

#include "GLRenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff2 {
    struct CallbackVideo {
        static const std::function<void(SettingsMenu &, GLRenderScene &)> DATA_SETTINGS;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> ANIMATION_SETTINGS;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> EXPORT_SETTINGS;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> GENERATE_VID_KEYFRAME;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> EXPORT_ZOOM_VID;
    };
}
