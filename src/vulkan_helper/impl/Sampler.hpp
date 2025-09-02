//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class SamplerImpl final : public CoreHandler {

        VkSampler sampler = VK_NULL_HANDLE;
        const VkSamplerCreateInfo samplerInfo;

    public:
        explicit SamplerImpl(const CoreRef core, const VkSamplerCreateInfo &samplerInfo);

        ~SamplerImpl() override;

        SamplerImpl(const SamplerImpl &other) = delete;

        SamplerImpl &operator=(const SamplerImpl &other) = delete;

        SamplerImpl(SamplerImpl &&other) = delete;

        SamplerImpl &operator=(SamplerImpl &&other) = delete;

        [[nodiscard]] VkSampler getSamplerHandle() const { return sampler; }

    private:
        void init() override;

        void destroy() override;
    };

    
    using Sampler = std::unique_ptr<SamplerImpl>;
    using SamplerPtr = SamplerImpl *;
    using SamplerRef = SamplerImpl &;
}
