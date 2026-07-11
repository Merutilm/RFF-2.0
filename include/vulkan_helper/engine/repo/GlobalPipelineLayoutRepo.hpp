//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vulkan_helper/engine/pipeline/PipelineLayout.hpp>
#include <vulkan_helper/hash/PipelineLayoutBuilderHasher.hpp>
#include <vulkan_helper/engine/manage/PipelineLayoutManager.hpp>
#include <vulkan_helper/engine/repo/Repository.hpp>

namespace merutilm::vkh {
    struct GlobalPipelineLayoutRepo final : Repository<PipelineLayoutBuilder, PipelineLayoutManager &&, std::unique_ptr<PipelineLayout>,
                PipelineLayout &, PipelineLayoutBuilderHasher, std::equal_to<>> {
        using Repository::Repository;

        PipelineLayout & pick(PipelineLayoutManager &&layoutManager) override;
    };
}
