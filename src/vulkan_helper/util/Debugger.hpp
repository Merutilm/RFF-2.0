//
// Created by Merutilm on 2025-08-24.
//
#pragma once
#include "../core/vkh_core.hpp"

namespace merutilm::vkh {
    struct Debugger {
        explicit Debugger() = delete;

        static constexpr auto VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";

        static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo() {
            return {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = 0,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                .pfnUserCallback = debugCallback,
                .pUserData = nullptr
            };
        }


        static VkBool32 debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                      [[maybe_unused]] void *pUserData) {
            const char *severityStr = nullptr;
            switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                    severityStr = "[Verbose]";
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                    severityStr = "[Info]";
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                    severityStr = "[Warning]";
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                    severityStr = "[Error]";
                    break;
                }
                default: {
                    severityStr = "[Unknown]";
                    break;
                }
            }

            const char *messageTypeStr;
            switch (messageType) {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                    messageTypeStr = "[General]";
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                    messageTypeStr = "[Validation]";
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                    messageTypeStr = "[Performance]";
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: {
                    messageTypeStr = "[DeviceAddressBinding]";
                    break;
                }
                default: {
                    messageTypeStr = "[Unknown]";
                    break;
                }
            }
            if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                logger::log_err_silent("{} {} : {}", severityStr, messageTypeStr, pCallbackData->pMessage);
            }

            return VK_FALSE;
        }
    };
}
