//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "../handle/Handler.hpp"

namespace merutilm::vkh {
    class ValidationLayerImpl final : public Handler {
        VkInstance instance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        bool enabled;

    public:

        explicit ValidationLayerImpl(VkInstance instance, bool enabled);

        ~ValidationLayerImpl() override;

        ValidationLayerImpl(const ValidationLayerImpl &) = delete;

        ValidationLayerImpl &operator=(const ValidationLayerImpl &) = delete;

        ValidationLayerImpl(ValidationLayerImpl &&) = delete;

        ValidationLayerImpl &operator=(ValidationLayerImpl &&) = delete;

        [[nodiscard]] bool isEnabled() const { return enabled; }


    private:
        void checkValidationLayerSupport() const;

        void init() override;

        void setupDebugMessenger();

        static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                     const VkAllocationCallbacks *pAllocator,
                                                     VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                                  VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks *pAllocator);


        void destroy() override;
    };

    using ValidationLayer = std::unique_ptr<ValidationLayerImpl>;
    using ValidationLayerPtr = ValidationLayerImpl *;
    using ValidationLayerRef = ValidationLayerImpl &;

}
