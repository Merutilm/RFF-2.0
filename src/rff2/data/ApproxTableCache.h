//
// Created by Merutilm on 2025-05-23.
//

#pragma once
#include "../mrthy/DeepPA.h"
#include "../mrthy/LightPA.h"

namespace merutilm::rff2 {
    struct ApproxTableCache {

        std::vector<std::vector<LightPA>> mpaLightTable = std::vector<std::vector<LightPA>>();
        std::vector<std::vector<DeepPA>> mpaDeepTable = std::vector<std::vector<DeepPA>>();

        std::vector<size_t> mpaIndexer = std::vector<size_t>();
        std::vector<uint64_t> mpaElementCounter = std::vector<uint64_t>();
    };
}