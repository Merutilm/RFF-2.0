//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "../handle/Handler.hpp"
#include "ValidationLayer.hpp"

namespace merutilm::vkh {
    class InstanceImpl final : public Handler {
        VkInstance instance = nullptr;
        ValidationLayer validationLayer;
        bool enableValidationLayer;
        std::vector<const char *> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        };

    public:
        explicit InstanceImpl(bool enableValidationLayer);

        ~InstanceImpl() override;

        InstanceImpl(const InstanceImpl &) = delete;

        InstanceImpl &operator=(const InstanceImpl &) = delete;

        InstanceImpl(InstanceImpl &&) = delete;

        InstanceImpl &operator=(InstanceImpl &&) = delete;

        [[nodiscard]] VkInstance getInstanceHandle() const { return instance; }

        [[nodiscard]] const ValidationLayerRef getValidationLayer() const { return *validationLayer; }

    private:
        void init() override;

        void createInstance();

        void destroy() override;
    };

    using Instance = std::unique_ptr<InstanceImpl>;
    using InstancePtr = InstanceImpl *;
    using InstanceRef = InstanceImpl &;
}
