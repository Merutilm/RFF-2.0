
//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include "../manage/PipelineLayoutManager.hpp"
#include "UniquePtrHasher.hpp"
#include "VectorHasher.hpp"


namespace merutilm::vkh {

    struct PipelineLayoutManagerPtrHasher{
        using is_transparent = void;

        size_t operator()(const PipelineLayoutManagerPtr key) const {
            size_t seed = 0;
            constexpr auto descHasher = VectorHasher<DescriptorSetLayoutPtr>();
            BoostHasher::hash(descHasher(key->getDescriptorSetLayouts()), &seed);

            constexpr auto pushMngPtrHasher = VectorHasher<PushConstantManager, UniquePtrHasher>();
            BoostHasher::hash(pushMngPtrHasher(key->getPushConstantManagers()), &seed);
            return seed;
        }
    };
}
