//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "RFFRenderScene.h"
#include "RFFSettingsMenu.h"

struct RFFCallbackShader {
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> PALETTE;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> STRIPE;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> SLOPE;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> COLOR;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> FOG;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> BLOOM;

};
