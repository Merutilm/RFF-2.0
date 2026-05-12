//
// Created by Merutilm on 2025-07-12.
//

#pragma once

namespace merutilm::vkh {
    struct DescriptorSetLayoutBuildType {
        VkDescriptorType type;
        VkShaderStageFlags stage;

        bool operator==(const DescriptorSetLayoutBuildType &) const = default;
    };
}
