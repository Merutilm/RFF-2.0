//
// Created by Merutilm on 2025-07-18.
//

#include "CombinedImageSampler.hpp"

#include "../util/ImageContextUtils.hpp"

namespace merutilm::vkh {
    CombinedImageSamplerImpl::CombinedImageSamplerImpl(CoreRef core, SamplerRef sampler) : CoreHandler(core), sampler(sampler) {
        CombinedImageSamplerImpl::init();
    }


    CombinedImageSamplerImpl::~CombinedImageSamplerImpl() {
        CombinedImageSamplerImpl::destroy();
    }


    const ImageContext &CombinedImageSamplerImpl::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        return imageContext;
    }


    void CombinedImageSamplerImpl::init() {
        //no operation
    }

    void CombinedImageSamplerImpl::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        if (isUnique) {
            vkDestroyImageView(device, imageContext.imageView, nullptr);
            vkDestroyImage(device, imageContext.image, nullptr);
            vkFreeMemory(device, imageContext.imageMemory, nullptr);
        }
    }
}
