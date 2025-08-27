//
// Created by Merutilm on 2025-07-18.
//

#include "CombinedMultiframeImageSampler.hpp"

#include "../util/ImageContextUtils.hpp"

namespace merutilm::vkh {
    CombinedMultiframeImageSamplerImpl::CombinedMultiframeImageSamplerImpl(
        CoreRef core, SamplerRef sampler) : CoreHandler(core), sampler(sampler) {
        CombinedMultiframeImageSamplerImpl::init();
    }


    CombinedMultiframeImageSamplerImpl::~CombinedMultiframeImageSamplerImpl() {
        CombinedMultiframeImageSamplerImpl::destroy();
    }


    const MultiframeImageContext &CombinedMultiframeImageSamplerImpl::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        return imageContext;
    }


    void CombinedMultiframeImageSamplerImpl::init() {
        //no operation
    }

    void CombinedMultiframeImageSamplerImpl::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        if (isUnique) {
            for (int i = 0; i < core.getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                vkDestroyImageView(device, imageContext[i].writeImageView, nullptr);
                vkDestroyImage(device, imageContext[i].image, nullptr);
                vkFreeMemory(device, imageContext[i].imageMemory, nullptr);
            }
        }
    }
}
