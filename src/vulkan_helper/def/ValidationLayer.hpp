//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include <vulkan/vulkan_core.h>

#include "../handle/Handler.hpp"

namespace merutilm::vkh {
    class ValidationLayer final : public Handler {
        VkInstance instance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        bool enabled;

    public:
        static constexpr auto VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";

        explicit ValidationLayer(VkInstance instance, bool enabled);

        ~ValidationLayer() override;

        ValidationLayer(const ValidationLayer &) = delete;

        ValidationLayer &operator=(const ValidationLayer &) = delete;

        ValidationLayer(ValidationLayer &&) = delete;

        ValidationLayer &operator=(ValidationLayer &&) = delete;

        [[nodiscard]] bool isEnabled() const { return enabled; }


        static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();

    private:
        void checkValidationLayerSupport() const;

        void init() override;

        void setupDebugMessenger();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            [[maybe_unused]] void *pUserData);

        static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                     const VkAllocationCallbacks *pAllocator,
                                                     VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                                  VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks *pAllocator);


        void destroy() override;
    };

}
