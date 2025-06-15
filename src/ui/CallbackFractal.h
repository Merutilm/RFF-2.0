//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff {
    struct CallbackFractal {
        static const std::function<void(SettingsMenu&, RenderScene&)> REFERENCE;
        static const std::function<void(SettingsMenu&, RenderScene&)> ITERATIONS;
        static const std::function<void(SettingsMenu&, RenderScene&)> MPA;
        static const std::function<bool*(RenderScene&)> AUTOMATIC_ITERATIONS;
        static const std::function<bool*(RenderScene&)> ABSOLUTE_ITERATION_MODE;
    };
}