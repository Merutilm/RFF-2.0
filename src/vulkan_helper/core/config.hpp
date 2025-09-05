//
// Created by Merutilm on 2025-08-29.
//

#pragma once
#include <vulkan/vulkan.h>

namespace merutilm::vkh::config {
#ifdef NDEBUG
    static constexpr bool ENABLE_VALIDATION = false;
#else
    static constexpr bool ENABLE_VALIDATION = true;
#endif

    static constexpr VkFormat SWAPCHAIN_IMAGE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;
    static constexpr auto DUMMY_WINDOW_CLASS = L"VulkanDummyWindowContext";
    static constexpr float INITIAL_FPS = 60.0f;
}
