//
// Created by Merutilm on 2025-07-22.
//

#pragma once

#include <vulkan_helper/base/pch.hpp>
namespace merutilm::vkh {

    template<typename T>
    struct PushConstantReserve {
        using type = T;
        uint32_t binding;
    };
}
