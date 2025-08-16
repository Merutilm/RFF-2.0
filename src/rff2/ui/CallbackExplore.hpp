//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <functional>

#include "RenderScene.hpp"
#include "SettingsMenu.hpp"

namespace merutilm::rff2 {
    struct CallbackExplore {
        static const std::function<void(SettingsMenu &, RenderScene &)> RECOMPUTE;
        static const std::function<void(SettingsMenu &, RenderScene &)> RESET;
        static const std::function<void(SettingsMenu &, RenderScene &)> CANCEL_RENDER;
        static const std::function<void(SettingsMenu &, RenderScene &)> FIND_CENTER;
        static const std::function<void(SettingsMenu &, RenderScene &)> LOCATE_MINIBROT;

        static std::function<void(uint64_t, int)> getActionWhileFindingMinibrotCenter(const RenderScene &scene, float logZoom, uint64_t longestPeriod);

        static std::function<void(uint64_t, float)> getActionWhileCreatingTable(const RenderScene &scene, float logZoom);

        static std::function<void(float)> getActionWhileFindingZoom(const RenderScene &scene);
    };
}
