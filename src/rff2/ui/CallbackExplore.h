//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <functional>

#include "SettingsMenu.h"

namespace merutilm::rff2 {
    struct CallbackExplore {
        static const std::function<void(SettingsMenu &, GLRenderScene &)> RECOMPUTE;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> RESET;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> CANCEL_RENDER;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> FIND_CENTER;
        static const std::function<void(SettingsMenu &, GLRenderScene &)> LOCATE_MINIBROT;

        static std::function<void(uint64_t, int)> getActionWhileFindingMinibrotCenter(const GLRenderScene &scene, float logZoom, uint64_t longestPeriod);

        static std::function<void(uint64_t, float)> getActionWhileCreatingTable(const GLRenderScene &scene, float logZoom);

        static std::function<void(float)> getActionWhileFindingZoom(const GLRenderScene &scene);
    };
}
