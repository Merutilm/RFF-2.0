//
// Created by Merutilm on 2025-08-13.
//

#include <vulkan_helper/engine/sampler/Sampler.hpp>

namespace merutilm::vkh {
    Sampler::Sampler(Core & core, const VkSamplerCreateInfo &samplerInfo) : CoreHandler(core), samplerInfo(samplerInfo) {
        Sampler::init();
    }

    Sampler::~Sampler() {
        Sampler::cleanup();
    }

    void Sampler::init() {
        if (vkCreateSampler(core.getLogicalDevice().getLogicalDeviceHandle(), &samplerInfo, nullptr, &sampler)) {
            throw exception_init("Failed to create sampler!");
        }
    }

    void Sampler::cleanup() {
        vkDestroySampler(core.getLogicalDevice().getLogicalDeviceHandle(), sampler, nullptr);
    }
}
