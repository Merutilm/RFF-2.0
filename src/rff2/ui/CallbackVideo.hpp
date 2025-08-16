//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <functional>

#include "RenderScene.hpp"
#include "SettingsMenu.hpp"

namespace merutilm::rff2 {
    struct CallbackVideo {
        static const std::function<void(SettingsMenu &, RenderScene &)> DATA_SETTINGS;
        static const std::function<void(SettingsMenu &, RenderScene &)> ANIMATION_SETTINGS;
        static const std::function<void(SettingsMenu &, RenderScene &)> EXPORT_SETTINGS;
        static const std::function<void(SettingsMenu &, RenderScene &)> GENERATE_VID_KEYFRAME;
        static const std::function<void(SettingsMenu &, RenderScene &)> EXPORT_ZOOM_VID;
    };
}
