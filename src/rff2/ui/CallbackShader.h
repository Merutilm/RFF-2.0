//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "GLRenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff2 {
    struct CallbackShader {
        static const std::function<void(SettingsMenu &, GLRenderScene &)> PALETTE;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> STRIPE;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> SLOPE;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> COLOR;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> FOG;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> BLOOM;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> LOAD_KFR_PALETTE;
    };
}
