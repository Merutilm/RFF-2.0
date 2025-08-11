//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "GLRenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff2 {
    struct CallbackFile {
        static const std::function<void(SettingsMenu &, GLRenderScene &)> SAVE_MAP;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> SAVE_IMAGE;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> SAVE_LOCATION;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> LOAD_MAP;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> LOAD_LOCATION;
    };
}
