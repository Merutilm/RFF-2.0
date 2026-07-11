//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vulkan_helper/engine/descriptor/DescriptorSetLayout.hpp>
#include <vulkan_helper/engine/pipeline/PushConstant.hpp>

namespace merutilm::vkh {

    using PipelineLayoutBuildType = std::variant<DescriptorSetLayout *, PushConstant *>;
    using PipelineLayoutBuilder = std::vector<PipelineLayoutBuildType>;

    struct PipelineLayoutManager {
        PipelineLayoutBuilder builders;
        uint32_t descriptorSetLayoutCount = 0;

        PipelineLayoutManager();

        ~PipelineLayoutManager();

        PipelineLayoutManager(const PipelineLayoutManager &) = delete;

        PipelineLayoutManager &operator=(const PipelineLayoutManager &) = delete;

        PipelineLayoutManager(PipelineLayoutManager &&) noexcept = default;

        PipelineLayoutManager &operator=(PipelineLayoutManager &&) = delete;

        bool operator==(const PipelineLayoutManager &) const;

        void appendDescriptorSetLayout(DescriptorSetLayout * descriptorSetLayout);

        void appendPushConstantManager(PushConstant * pushConstant);
    };



}
