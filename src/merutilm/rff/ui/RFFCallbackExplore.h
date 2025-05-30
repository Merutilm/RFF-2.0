//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <functional>

#include "RFFSettingsMenu.h"

struct RFFCallbackExplore {
    static const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RECOMPUTE;
    static const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> REFRESH_COLOR;
    static const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RESET;
    static const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> CANCEL_RENDER;
    static const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> FIND_CENTER;
    static const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> LOCATE_MINIBROT;

    static std::function<void(uint64_t, int)> getActionWhileFindingMinibrotCenter(const RFFRenderScene &scene, float logZoom, uint64_t longestPeriod);

    static std::function<void(uint64_t, float)> getActionWhileCreatingTable(const RFFRenderScene &scene, float logZoom);

    static std::function<void(float)> getActionWhileFindingZoom(const RFFRenderScene &scene);
};

