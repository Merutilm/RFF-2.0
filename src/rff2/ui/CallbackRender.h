//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include "GLRenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff2 {
    struct CallbackRender {
        static const std::function<void(SettingsMenu &, GLRenderScene &)> SET_CLARITY;
        static const std::function<bool*(GLRenderScene &)> ANTIALIASING;
    };
}
