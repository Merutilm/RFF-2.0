//
// Created by Merutilm on 2025-07-08.
//

#pragma once

#include "../handle/Handler.hpp"
#include "ValidationLayer.hpp"

namespace merutilm::vkh {
    class InstanceImpl final : public Handler {
        VkInstance instance = nullptr;
        ValidationLayer validationLayer;

        std::vector<const char *> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        };

    public:
        explicit InstanceImpl();

        ~InstanceImpl() override;

        InstanceImpl(const InstanceImpl &) = delete;

        InstanceImpl &operator=(const InstanceImpl &) = delete;

        InstanceImpl(InstanceImpl &&) = delete;

        InstanceImpl &operator=(InstanceImpl &&) = delete;

        [[nodiscard]] VkInstance getInstanceHandle() const { return instance; }


    private:
        void init() override;

        void createInstance();

        void destroy() override;
    };

    using Instance = std::unique_ptr<InstanceImpl>;
    using InstancePtr = InstanceImpl *;
    using InstanceRef = InstanceImpl &;
}
