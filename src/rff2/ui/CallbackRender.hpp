//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include "RenderScene.hpp"
#include "SettingsMenu.hpp"

namespace merutilm::rff2 {
    struct CallbackRender {
        static const std::function<void(SettingsMenu &, RenderScene &)> SET_CLARITY;
        static const std::function<bool*(RenderScene &, bool)> LINEAR_INTERPOLATION;
    };
}
