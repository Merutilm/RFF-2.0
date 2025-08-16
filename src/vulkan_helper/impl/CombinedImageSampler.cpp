//
// Created by Merutilm on 2025-07-18.
//

#include "CombinedImageSampler.hpp"

#include "../util/ImageContextUtils.hpp"

namespace merutilm::vkh {
    CombinedImageSampler::CombinedImageSampler(const Core &core, const Sampler &sampler) : CoreHandler(core), sampler(sampler) {
        CombinedImageSampler::init();
    }


    CombinedImageSampler::~CombinedImageSampler() {
        CombinedImageSampler::destroy();
    }


    const ImageContext &CombinedImageSampler::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        return imageContext;
    }


    void CombinedImageSampler::init() {
        //no operation
    }

    void CombinedImageSampler::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        if (isUnique) {
            vkDestroyImageView(device, imageContext.imageView, nullptr);
            vkDestroyImage(device, imageContext.image, nullptr);
            vkFreeMemory(device, imageContext.imageMemory, nullptr);
        }
    }
}
