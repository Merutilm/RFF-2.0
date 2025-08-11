//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "GLRenderScene.h"
#include "SettingsMenu.h"

namespace merutilm::rff2 {
    struct CallbackFractal {
        static const std::function<void(SettingsMenu&, GLRenderScene&)> REFERENCE;
        static const std::function<void(SettingsMenu&, GLRenderScene&)> ITERATIONS;
        static const std::function<void(SettingsMenu&, GLRenderScene&)> MPA;
        static const std::function<bool*(GLRenderScene&)> AUTOMATIC_ITERATIONS;
        static const std::function<bool*(GLRenderScene&)> ABSOLUTE_ITERATION_MODE;
    };
}