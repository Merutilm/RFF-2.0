//
// Created by Merutilm on 2025-09-05.
//



#pragma once
#include "../core/vkh_base.hpp"
#include <any>

namespace merutilm::vkh {
    struct BufferInitInfo {
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;
        std::vector<std::any> bufExtensions;
        std::vector<std::any> memExtensions;
    };


}
