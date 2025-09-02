//
// Created by Merutilm on 2025-09-01.
//
#pragma once
#include "../manage/PipelineLayoutManager.hpp"

namespace merutilm::vkh {
    struct PipelineLayoutManagerEquals {

        using is_transparent = void;

        bool operator()(PipelineLayoutManagerRef lhs, PipelineLayoutManagerRef rhs) const {
            return false;
        }
    };
}
