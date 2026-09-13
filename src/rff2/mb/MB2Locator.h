//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../parallel/ParallelRenderState.h"
#include "MB2RenderData.hpp"

namespace merutilm::rff2 {
    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 2.f;

        fixed_point_complex center;
        double logZoom;

        static std::unique_ptr<fixed_point_complex> findCenterOffset(const MB2ReferenceBase &reference);

        static std::optional<MB2Locator> locateMinibrot(const ParallelRenderState &state, const MB2RenderDataBase &data,
                       const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter);

    private:
        static std::unique_ptr<MB2ReferenceBase>
        findAccurateCenterReference(const ParallelRenderState &state, const MB2RenderDataBase &data,
                                    const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter);

        static bool checkMaxIterationOnly(const MB2RenderDataBase &renderData);
    };
}
