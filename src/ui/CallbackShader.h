//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "RenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff {
    struct CallbackShader {
        static const std::function<void(SettingsMenu&, RenderScene&)> PALETTE;
        static const std::function<void(SettingsMenu&, RenderScene&)> STRIPE;
        static const std::function<void(SettingsMenu&, RenderScene&)> SLOPE;
        static const std::function<void(SettingsMenu&, RenderScene&)> COLOR;
        static const std::function<void(SettingsMenu&, RenderScene&)> FOG;
        static const std::function<void(SettingsMenu&, RenderScene&)> BLOOM;

    };
}
