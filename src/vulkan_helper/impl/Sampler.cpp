//
// Created by Merutilm on 2025-08-13.
//

#include "Sampler.hpp"

#include "../core/allocator.hpp"

namespace merutilm::vkh {
    SamplerImpl::SamplerImpl(const CoreRef core, const VkSamplerCreateInfo &samplerInfo) : CoreHandler(core), samplerInfo(samplerInfo) {
        SamplerImpl::init();
    }

    SamplerImpl::~SamplerImpl() {
        SamplerImpl::destroy();
    }

    void SamplerImpl::init() {
        if (allocator::invoke(vkCreateSampler, core.getLogicalDevice().getLogicalDeviceHandle(), &samplerInfo, nullptr, &sampler)) {
            throw exception_init("Failed to create sampler!");
        }
    }

    void SamplerImpl::destroy() {
        allocator::invoke(vkDestroySampler, core.getLogicalDevice().getLogicalDeviceHandle(), sampler, nullptr);
    }
}
