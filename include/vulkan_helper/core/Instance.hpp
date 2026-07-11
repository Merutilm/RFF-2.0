//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include "ValidationLayer.hpp"
#include <vulkan_helper/handle/Handler.hpp>

namespace merutilm::vkh {
    class Instance final : public Handler {
        VkInstance instance = nullptr;
        std::unique_ptr<ValidationLayer> validationLayer;
        std::vector<const char *> additionalExtensions;

    public:
        explicit Instance(std::vector<const char *> &&additionalExtensions = {});

        ~Instance() override;

        Instance(const Instance &) = delete;

        Instance &operator=(const Instance &) = delete;

        Instance(Instance &&) = delete;

        Instance &operator=(Instance &&) = delete;

        [[nodiscard]] VkInstance getInstanceHandle() const { return instance; }

    protected:
        void init() override;

        void cleanup() override;

    private:
        void createInstance();
    };


}
