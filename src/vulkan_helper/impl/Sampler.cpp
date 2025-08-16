//
// Created by Merutilm on 2025-08-13.
//

#include "Sampler.hpp"

namespace merutilm::vkh {
    Sampler::Sampler(const Core &core, VkSamplerCreateInfo &&samplerInfo) : CoreHandler(core), samplerInfo(std::move(samplerInfo)) {
        Sampler::init();
    }

    Sampler::~Sampler() {
        Sampler::destroy();
    }

    void Sampler::init() {
        if (vkCreateSampler(core.getLogicalDevice().getLogicalDeviceHandle(), &samplerInfo, nullptr, &sampler)) {
            throw exception_init("Failed to create sampler!");
        }
    }

    void Sampler::destroy() {
        vkDestroySampler(core.getLogicalDevice().getLogicalDeviceHandle(), sampler, nullptr);
    }
}
