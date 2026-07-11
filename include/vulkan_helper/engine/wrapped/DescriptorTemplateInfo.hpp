//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <vulkan_helper/engine/manage/DescriptorManager.hpp>

namespace merutilm::vkh {
    struct DescriptorTemplateInfo {
        uint32_t id;
        std::function<std::vector<DescriptorManager>(Core &)> descriptorGenerator;
    };
}
