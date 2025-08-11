//
// Created by Merutilm on 2025-07-23.
//

#pragma once
#include "BoostHasher.hpp"
#include "../struct/DescriptorSetLayoutBuildType.hpp"

namespace merutilm::vkh {
    struct DescriptorSetLayoutBuildTypeHasher {
        using is_transparent = void;

        size_t operator()(const DescriptorSetLayoutBuildType &buildType) const {
            size_t seed = 0;
            BoostHasher::hash(buildType.stage, &seed);
            BoostHasher::hash(buildType.type, &seed);
            return seed;
        }
    };
}
