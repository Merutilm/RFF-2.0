//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "../manage/DescriptorManager.hpp"

namespace merutilm::mvk {

    struct DescriptorTemplateInfo {
        uint32_t id;
        std::function<std::unique_ptr<DescriptorManager>(const Core &, VkShaderStageFlags)> descriptorGenerator;
    };
}
