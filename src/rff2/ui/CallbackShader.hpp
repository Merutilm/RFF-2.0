//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "RenderScene.hpp"
#include "SettingsMenu.hpp"

namespace merutilm::rff2 {
    struct CallbackShader {
        static const std::function<void(SettingsMenu &, RenderScene &)> PALETTE;
        static const std::function<void(SettingsMenu &, RenderScene &)> STRIPE;
        static const std::function<void(SettingsMenu &, RenderScene &)> SLOPE;
        static const std::function<void(SettingsMenu &, RenderScene &)> COLOR;
        static const std::function<void(SettingsMenu &, RenderScene &)> FOG;
        static const std::function<void(SettingsMenu &, RenderScene &)> BLOOM;
        static const std::function<void(SettingsMenu &, RenderScene &)> LOAD_KFR_PALETTE;
    };
}
