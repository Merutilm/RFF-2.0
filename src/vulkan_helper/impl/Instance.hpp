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
    class Instance final : public Handler {
        VkInstance instance = nullptr;
        std::unique_ptr<ValidationLayer> validationLayer;
        bool enableValidationLayer;
        std::vector<const char *> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        };

    public:
        explicit Instance(bool enableValidationLayer);

        ~Instance() override;

        Instance(const Instance &) = delete;

        Instance &operator=(const Instance &) = delete;

        Instance(Instance &&) = delete;

        Instance &operator=(Instance &&) = delete;

        [[nodiscard]] VkInstance getInstanceHandle() const { return instance; }

        [[nodiscard]] const ValidationLayer &getValidationLayer() const { return *validationLayer; }

    private:
        void init() override;

        void createInstance();

        void destroy() override;
    };
}
