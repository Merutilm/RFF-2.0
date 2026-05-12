//
// Created by Merutilm on 2025-09-01.
//
#pragma once
#include <vulkan_helper/engine/manage/PipelineLayoutManager.hpp>

namespace merutilm::vkh {
    struct PipelineLayoutManagerEquals {

        using is_transparent = void;

        bool operator()(PipelineLayoutManager & lhs, PipelineLayoutManager & rhs) const {
            return false;
        }
    };
}
