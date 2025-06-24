//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff {
    struct CallbackFile {
        static const std::function<void(SettingsMenu &, RenderScene &)> SAVE_MAP;
        static const std::function<void(SettingsMenu &, RenderScene &)> SAVE_IMAGE;
        static const std::function<void(SettingsMenu &, RenderScene &)> SAVE_LOCATION;
        static const std::function<void(SettingsMenu &, RenderScene &)> LOAD_MAP;
        static const std::function<void(SettingsMenu &, RenderScene &)> LOAD_LOCATION;
    };
}
