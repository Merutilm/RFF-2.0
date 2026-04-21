//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../calc/fp_complex.h"
#include "../data/ApproxTableManager.h"
#include "../formula/LightMB2Perturbator.h"

namespace merutilm::rff2 {
    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 1.5f;
        static constexpr float ZOOM_INCREMENT_LIMIT = 0.01f;

        std::unique_ptr<MB2Perturbator> perturbator;

        static std::unique_ptr<fp_complex> findCenter(const MB2Perturbator *perturbator);

        static std::unique_ptr<fp_complex> findCenterOffset(const MB2Perturbator &perturbator);

        static std::unique_ptr<MB2Locator> locateMinibrot(ParallelRenderState &state,
                                                                 const MB2Perturbator *perturbator,
                                                                 ApproxTableManager &approxTableCache,
                                                                 const std::function<void(uint64_t, int)> &
                                                                 actionWhileFindingMinibrotCenter,
                                                                 const std::function<void(uint64_t, float)> &actionWhileCreatingTable,
                                                                 const std::function<void(float)> &actionWhileFindingMinibrotZoom);

    private:
        static std::unique_ptr<MB2Perturbator> findAccurateCenterPerturbator(ParallelRenderState &state,
            const MB2Perturbator *perturbator,
            ApproxTableManager &approxTableCache,
            const std::function<void(uint64_t, int)> &
            actionWhileFindingMinibrotCenter, const std::function<void(uint64_t, float)> &
            actionWhileCreatingTable);

        static bool checkMaxIterationOnly(const MB2Perturbator &perturbator, uint64_t maxIteration);
    };
}
