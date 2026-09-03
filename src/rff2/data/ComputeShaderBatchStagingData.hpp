//
// Created by Merutilm on 8/27/26.
//

#pragma once
#include <cstdint>

#include "../calc/complex.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    struct ComputeShaderBatchStagingData {
        uint64_t iteration = 0;
        uint64_t refIteration = 0;
        complex<Num> dz = {Num(0), Num(0)};
        float distance2 = 0;
    };
}