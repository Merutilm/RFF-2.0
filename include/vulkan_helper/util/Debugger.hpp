//
// Created by Merutilm on 2025-08-24.
//
#pragma once
#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    struct Debugger {
        explicit Debugger() = delete;

        static constexpr auto VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";

        static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();


        static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                      [[maybe_unused]] void *pUserData);
    };
}
