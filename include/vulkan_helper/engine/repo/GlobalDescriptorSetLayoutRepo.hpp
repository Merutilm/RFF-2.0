//
// Created by Merutilm on 2025-07-16.
//

#pragma once

#include <vulkan_helper/engine/descriptor/DescriptorSetLayout.hpp>
#include "Repository.hpp"

namespace merutilm::vkh {
    struct GlobalDescriptorSetLayoutRepo final : Repository<DescriptorSetLayoutBuilder, const DescriptorSetLayoutBuilder &, std::unique_ptr<DescriptorSetLayout>,
                     DescriptorSetLayout &, DescriptorSetLayoutBuilderHasher, std::equal_to<>> {
        using Repository::Repository;

        DescriptorSetLayout &pick(const DescriptorSetLayoutBuilder &layoutBuilder) override;
    };


} // namespace merutilm::vkh
