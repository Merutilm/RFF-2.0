//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include "RenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff {
    struct CallbackRender {
        static const std::function<void(SettingsMenu &, RenderScene &)> SET_CLARITY;
        static const std::function<bool*(RenderScene &)> ANTIALIASING;
    };
}
