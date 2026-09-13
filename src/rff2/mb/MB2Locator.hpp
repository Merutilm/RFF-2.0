//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../parallel/ParallelRenderState.h"
#include "MB2RenderData.hpp"

namespace merutilm::rff2 {

    struct MB2LocateResult {
        fixed_point_complex center;
        double logZoom;
    };


    struct BlockResult
    {
        fixed_point_complex residual;
        fixed_point_complex an;
        fixed_point_complex bn;
        complex<dex> fzgAn;
    };

    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 2.f;

        static fixed_point_complex calcCenterOffset(const MB2ReferenceBase &reference);

        static void calcCenterOffset(fixed_point_complex &result, const fixed_point_complex &z,
                                     const fixed_point_complex &bn);

        static std::optional<MB2LocateResult> locateMinibrot(const ParallelRenderState &state, const MB2RenderDataBase &data,
                       const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter);

    };
}
