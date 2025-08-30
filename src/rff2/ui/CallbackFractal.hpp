//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RenderScene.hpp"
#include "SettingsMenu.hpp"

namespace merutilm::rff2 {
    struct CallbackFractal {
        static const std::function<void(SettingsMenu&, RenderScene&)> REFERENCE;
        static const std::function<void(SettingsMenu&, RenderScene&)> ITERATIONS;
        static const std::function<void(SettingsMenu&, RenderScene&)> MPA;
        static const std::function<bool*(RenderScene&, bool)> AUTOMATIC_ITERATIONS;
        static const std::function<bool*(RenderScene&, bool)> ABSOLUTE_ITERATION_MODE;
    };
}