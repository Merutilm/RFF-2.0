//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "../impl/PushConstant.hpp"
#include "../impl/DescriptorSetLayout.hpp"

namespace merutilm::vkh {

    using PipelineLayoutBuildType = std::variant<DescriptorSetLayoutPtr, PushConstantPtr>;
    using PipelineLayoutBuilder = std::vector<PipelineLayoutBuildType>;

    struct PipelineLayoutManagerImpl {
        PipelineLayoutBuilder builders;
        uint32_t descriptorSetLayoutCount;

        PipelineLayoutManagerImpl() = default;

        ~PipelineLayoutManagerImpl() = default;

        PipelineLayoutManagerImpl(const PipelineLayoutManagerImpl &) = delete;

        PipelineLayoutManagerImpl &operator=(const PipelineLayoutManagerImpl &) = delete;

        PipelineLayoutManagerImpl(PipelineLayoutManagerImpl &&) = delete;

        PipelineLayoutManagerImpl &operator=(PipelineLayoutManagerImpl &&) = delete;

        bool operator==(const PipelineLayoutManagerImpl &) const = default;

        void appendDescriptorSetLayout(DescriptorSetLayoutPtr descriptorSetLayout) {
            builders.emplace_back(descriptorSetLayout);
            ++descriptorSetLayoutCount;
        }

        void appendPushConstantManager(PushConstantPtr pushConstant) {
            builders.emplace_back(std::move(pushConstant));
        }
    };

    using PipelineLayoutManager = std::unique_ptr<PipelineLayoutManagerImpl>;
    using PipelineLayoutManagerPtr = PipelineLayoutManagerImpl *;
    using PipelineLayoutManagerRef = PipelineLayoutManagerImpl &;
}
