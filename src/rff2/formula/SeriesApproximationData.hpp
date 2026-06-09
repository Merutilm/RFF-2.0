//
// Created by Merutilm on 2026-06-01.
//

#pragma once
#include <cstdint>
#include <vector>
#include "../calc/complex.hpp"
namespace merutilm::rff2 {
    struct SeriesApproximationData {

        std::vector<complex<dex>> terms;
        uint64_t skippedIterations;
    };
}
