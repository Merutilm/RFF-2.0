//
// Created by Merutilm on 8/27/26.
//

#pragma once
#include <cstdint>
#include <glm/glm.hpp>
namespace merutilm::rff2 {
    struct ComputeShaderBatchStagingData {
        uint64_t iteration = 0;
        uint64_t refIteration = 0;
        glm::vec2 dz = {0, 0};
        float distance2 = 0;
        uint32_t completed = 0;
    };
}