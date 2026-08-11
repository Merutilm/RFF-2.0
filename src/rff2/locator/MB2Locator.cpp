//
// Created by Merutilm on 2025-05-16.
// modified by Vinfinity on 2026-08-11!!
//

#pragma once
#include "../calc/fixed_point_complex.hpp"
#include "../formula/MB2RenderData.hpp"
#include "../parallel/ParallelRenderState.h"

namespace merutilm::rff2 {
    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 1.5f;
        static constexpr float ZOOM_INCREMENT_LIMIT = 0.01f;

        std::unique_ptr<MB2RenderDataBase> data;

        static std::unique_ptr<fixed_point_complex_i1> findCenter(const MB2RenderDataBase &data);

        static std::unique_ptr<fixed_point_complex_i1> findCenterOffset(const MB2RenderDataBase &data);

        static std::unique_ptr<MB2Locator> locateMinibrot(ParallelRenderState &state, const MB2RenderDataBase &data,
                       std::unique_ptr<ApproxTableCacheBase> &cache,
                       const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter,
                       const std::function<void(uint64_t, float)> &actionWhileSeriesApprox,
                       const std::function<void(uint64_t, float)> &actionWhileCreatingTable,
                       const std::function<void(float)> &actionWhileFindingMinibrotZoom);

        static std::unique_ptr<MB2RenderDataBase>
        locateMinibrotCenter(ParallelRenderState &state, const MB2RenderDataBase &data,
                             std::unique_ptr<ApproxTableCacheBase> &cache,
                             const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter,
                             const std::function<void(uint64_t, float)> &actionWhileSeriesApprox,
                             const std::function<void(uint64_t, float)> &actionWhileCreatingTable);

    private:
        static std::unique_ptr<MB2RenderDataBase>
        findAccurateCenterPerturbator(ParallelRenderState &state, const MB2RenderDataBase &data,
                                      std::unique_ptr<ApproxTableCacheBase> &cache,
                                      const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter,
                                      const std::function<void(uint64_t, float)> &actionWhileSeriesApprox,
                                      const std::function<void(uint64_t, float)> &actionWhileCreatingTable);

        static bool checkMaxIterationOnly(const MB2RenderDataBase &renderData);
    };
}
