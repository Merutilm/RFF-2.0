
//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include "../manage/PipelineLayoutManager.hpp"
#include "UniquePtrHasher.hpp"
#include "VariantHasher.hpp"
#include "VectorHasher.hpp"


namespace merutilm::vkh {

    struct PipelineLayoutBuilderHasher{
        using is_transparent = void;

        size_t operator()(const PipelineLayoutBuilder &key) const {
            size_t seed = 0;
            constexpr auto descHasher = VectorHasher<PipelineLayoutBuildType, VariantHasher>();
            BoostHasher::hash(descHasher(key), &seed);
            return seed;
        }
    };
}
