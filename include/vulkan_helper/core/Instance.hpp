//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include <vulkan_helper/handle/Handler.hpp>

#include "ApplicationCreateInfo.hpp"
#include "ValidationLayer.hpp"

namespace merutilm::vkh {
    class Instance final : public Handler {
        VkInstance instance = nullptr;
        const ApplicationCreateInfo &engineCreateInfo;
        std::unique_ptr<ValidationLayer> validationLayer;

    public:
        explicit Instance(const ApplicationCreateInfo &info);

        ~Instance() override;

        Instance(const Instance &) = delete;

        Instance &operator=(const Instance &) = delete;

        Instance(Instance &&) = delete;

        Instance &operator=(Instance &&) = delete;

        [[nodiscard]] VkInstance getInstanceHandle() const { return instance; }

        [[nodiscard]] const ApplicationCreateInfo &getApplicationCreateInfo() const { return engineCreateInfo; }

    protected:
        void init() override;

        void cleanup() override;

    private:
        void createInstance();
    };


} // namespace merutilm::vkh
