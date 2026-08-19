//
// Created by Merutilm on 8/19/26.
//

#pragma once
#include <cstdint>
namespace merutilm::rff2 {

    struct ShdNoiseReduction {
        bool use;
        uint32_t similarCountThreshold;
        float differenceThreshold;
    };
}