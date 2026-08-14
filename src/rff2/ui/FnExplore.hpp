//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include <cstdint>
#include <functional>


namespace merutilm::rff2 {

    class RFF2;
    struct FnExplore {
        static void recompute(RFF2 &app);
        static void reset(RFF2 &app);
        static void cancelRender(RFF2 &app);
        static void moveCursorToCenter(RFF2 &app);
        static void moveToCenter(RFF2 &app);
        static void reuseReference(RFF2 &app);
        static void goToOriginalReference(RFF2 &app);
        static void locateCenteredReference(RFF2 &app);
        static void locateMinibrot(RFF2 &app);

        static std::function<void(uint64_t, int)> getActionWhileFindingMBCenter(RFF2 &app,
                                                                                uint64_t longestPeriod, float startTime);

        static std::function<void(uint64_t, float)> getActionWhileSeriesApprox(RFF2 &app, float startTime);

        static std::function<void(uint64_t, float)> getActionWhileCreatingTable(RFF2 &app, float startTime);

        static std::function<void(float)> getActionWhileFindingZoom(RFF2 &app, float startTime);

        static std::function<void(uint64_t)> getActionWhileRefCalc(RFF2 &app, float startTime);
    };
} // namespace merutilm::rff2
