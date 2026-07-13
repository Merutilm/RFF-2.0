//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <cstdint>
#include <functional>


namespace merutilm::rff2 {

    class RFFApplication;
    struct FnExplore {
        static void recompute(RFFApplication &app);
        static void reset(RFFApplication &app);
        static void cancelRender(RFFApplication &app);
        static void findCenter(RFFApplication &app);
        static void locateMinibrot(RFFApplication &app);

        static std::function<void(uint64_t, int)> getActionWhileFindingMBCenter(RFFApplication &app,
                                                                                uint64_t longestPeriod);

        static std::function<void(uint64_t, float)> getActionWhileCreatingTable(RFFApplication &app);

        static std::function<void(float)> getActionWhileFindingZoom(RFFApplication &app);
    };
} // namespace merutilm::rff2
