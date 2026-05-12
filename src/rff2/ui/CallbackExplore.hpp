//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <functional>

#include "AppRenderManager.hpp"

namespace merutilm::rff2 {
    struct CallbackExplore {
        static std::function<void()> fnRecompute(AppRenderManager &arm);
        static std::function<void()> fnReset(AppRenderManager &arm);
        static std::function<void()> fnCancelRender(AppRenderManager &arm);
        static std::function<void()> fnFindCenter(AppRenderManager &arm);
        static std::function<void()> fnLocateMinibrot(AppRenderManager &arm);

        static std::function<void(uint64_t, int)>
        getActionWhileFindingMBCenter(const AppRenderManager &scene, float logZoom, uint64_t longestPeriod);

        static std::function<void(uint64_t, float)> getActionWhileCreatingTable(const AppRenderManager &scene,
                                                                                float logZoom);

        static std::function<void(float)> getActionWhileFindingZoom(const AppRenderManager &scene);
    };
} // namespace merutilm::rff2
