//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <functional>

#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct CallbackExplore {
        static std::function<void()> fnRecompute(RFFApplication &arm) {return []{};}
        static std::function<void()> fnReset(RFFApplication &arm){return []{};}
        static std::function<void()> fnCancelRender(RFFApplication &arm){return []{};}
        static std::function<void()> fnFindCenter(RFFApplication &arm){return []{};}
        static std::function<void()> fnLocateMinibrot(RFFApplication &arm){return []{};}

        static std::function<void(uint64_t, int)> getActionWhileFindingMBCenter(RFFApplication &app,
                                                                                uint64_t longestPeriod);

        static std::function<void(uint64_t, float)> getActionWhileCreatingTable(RFFApplication &app);

        static std::function<void(float)> getActionWhileFindingZoom(RFFApplication &scene);
    };
} // namespace merutilm::rff2
