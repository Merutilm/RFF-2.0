//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"
#include "../data/ApproxTableManager.h"
#include "../formula/LightMB2Perturbator.h"

namespace merutilm::rff2 {
    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 1.5f;
        static constexpr float ZOOM_INCREMENT_LIMIT = 0.01f;

        std::unique_ptr<MB2Perturbator> perturbator;

        static std::unique_ptr<fixed_point_complex_i1> findCenter(const MB2Perturbator *perturbator);

        static std::unique_ptr<fixed_point_complex_i1> findCenterOffset(const MB2Perturbator &perturbator);

        static std::unique_ptr<MB2Locator> locateMinibrot(ParallelRenderState &state,
                                                                 const MB2Perturbator *perturbator,
                                                                 const std::function<void(uint64_t, int)> &
                                                                 actionWhileFindingMinibrotCenter,
                                                                 const std::function<void(uint64_t, float)> &actionWhileCreatingTable,
                                                                 const std::function<void(float)> &actionWhileFindingMinibrotZoom);

    private:
        static std::unique_ptr<MB2Perturbator> findAccurateCenterPerturbator(ParallelRenderState &state,
            const MB2Perturbator *perturbator,
            const std::function<void(uint64_t, int)> &
            actionWhileFindingMinibrotCenter, const std::function<void(uint64_t, float)> &
            actionWhileCreatingTable);

        static bool checkMaxIterationOnly(const MB2Perturbator &perturbator, uint64_t maxIteration);
    };
}
