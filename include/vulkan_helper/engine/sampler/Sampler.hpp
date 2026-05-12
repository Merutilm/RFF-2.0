//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class Sampler final : public CoreHandler {

        VkSampler sampler = VK_NULL_HANDLE;
        const VkSamplerCreateInfo samplerInfo;

    public:
        explicit Sampler(Core & core, const VkSamplerCreateInfo &samplerInfo);

        ~Sampler() override;

        Sampler(const Sampler &other) = delete;

        Sampler &operator=(const Sampler &other) = delete;

        Sampler(Sampler &&other) = delete;

        Sampler &operator=(Sampler &&other) = delete;

        [[nodiscard]] VkSampler getSamplerHandle() const { return sampler; }

    private:
        void init() override;

        void cleanup() override;
    };

    

}
