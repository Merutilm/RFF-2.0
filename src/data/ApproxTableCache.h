//
// Created by Merutilm on 2025-05-23.
//

#pragma once
#include "../mrthy/DeepPA.h"
#include "../mrthy/LightPA.h"

namespace merutilm::rff2 {
    struct ApproxTableCache {
        std::vector<std::vector<LightPA>> lightTable = std::vector<std::vector<LightPA> >();
        std::vector<std::vector<DeepPA>> deepTable = std::vector<std::vector<DeepPA>>();
    };
}