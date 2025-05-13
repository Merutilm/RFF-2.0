//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RFFRenderScene.h"
#include "RFFSettingsMenu.h"

struct RFFCallbackFractal {
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> REFERENCE;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> ITERATIONS;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> MPA;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> AUTOMATIC_ITERATIONS;
    static const std::function<void(RFFSettingsMenu&, RFFRenderScene&)> ABSOLUTE_ITERATION_MODE;
};
