//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/handle/Handler.hpp>

namespace merutilm::vkh {
    class ValidationLayer final : public Handler {
        VkInstance instance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
    public:

        explicit ValidationLayer(VkInstance instance);

        ~ValidationLayer() override;

        ValidationLayer(const ValidationLayer &) = delete;

        ValidationLayer &operator=(const ValidationLayer &) = delete;

        ValidationLayer(ValidationLayer &&) = delete;

        ValidationLayer &operator=(ValidationLayer &&) = delete;


    protected:
        void init() override;

        void cleanup() override;

    private:
        static void checkValidationLayerSupport();

        void setupDebugMessenger();

        static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                     const VkAllocationCallbacks *pAllocator,
                                                     VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                                  VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks *pAllocator);


    };



}
