//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../formula/LightMandelbrotPerturbator.h"
#include "../precision/Center.h"

struct MandelbrotLocator {
    std::unique_ptr<LightMandelbrotPerturbator> perturbator;

    static std::unique_ptr<Center> findCenter(const LightMandelbrotPerturbator *perturbator);

    static std::unique_ptr<Center> findCenterOffset(const LightMandelbrotPerturbator &perturbator);

    static std::unique_ptr<MandelbrotLocator> locateMinibrot(ParallelRenderState &state,
                                                             std::unique_ptr<LightMandelbrotPerturbator> perturbator,
                                                             const std::function<void(uint64_t, int)> &
                                                             actionWhileFindingMinibrotCenter,
                                                             const std::function<void(uint64_t, float)> &actionWhileCreatingTable,
                                                             const std::function<void(float)> &actionWhileFindingMinibrotZoom);

private:
    static std::unique_ptr<LightMandelbrotPerturbator> findAccurateCenterPerturbator(ParallelRenderState &state,
        std::unique_ptr<LightMandelbrotPerturbator> perturbator,
        const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter,
        const std::function<void(uint64_t, float)> &actionWhileCreatingTable);

    static bool checkMaxIterationOnly(const LightMandelbrotPerturbator &perturbator, uint64_t maxIteration);
};
