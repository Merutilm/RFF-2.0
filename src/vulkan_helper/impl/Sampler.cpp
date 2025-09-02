//
// Created by Merutilm on 2025-08-13.
//

#include "Sampler.hpp"

namespace merutilm::vkh {
    SamplerImpl::SamplerImpl(const CoreRef core, const VkSamplerCreateInfo &samplerInfo) : CoreHandler(core), samplerInfo(samplerInfo) {
        SamplerImpl::init();
    }

    SamplerImpl::~SamplerImpl() {
        SamplerImpl::destroy();
    }

    void SamplerImpl::init() {
        if (vkCreateSampler(core.getLogicalDevice().getLogicalDeviceHandle(), &samplerInfo, nullptr, &sampler)) {
            throw exception_init("Failed to create sampler!");
        }
    }

    void SamplerImpl::destroy() {
        vkDestroySampler(core.getLogicalDevice().getLogicalDeviceHandle(), sampler, nullptr);
    }
}
