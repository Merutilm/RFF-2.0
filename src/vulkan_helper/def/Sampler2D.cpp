//
// Created by Merutilm on 2025-07-18.
//

#include "Sampler2D.hpp"

#include "../util/ImageContextUtils.hpp"

namespace merutilm::mvk {
    Sampler2D::Sampler2D(const Core &core, VkSamplerCreateInfo &&samplerInfo) : CoreHandler(core), samplerInfo(std::move(samplerInfo)) {
        Sampler2D::init();
    }


    Sampler2D::~Sampler2D() {
        Sampler2D::destroy();
    }


    const ImageContext &Sampler2D::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        return imageContext;
    }


    void Sampler2D::init() {

        if (vkCreateSampler(core.getLogicalDevice().getLogicalDeviceHandle(), &samplerInfo, nullptr, &sampler)) {
            throw exception_init("Failed to create sampler!");
        }
    }

    void Sampler2D::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkDestroySampler(device, sampler, nullptr);
        if (isUnique) {
            vkDestroyImageView(device, imageContext.imageView, nullptr);
            vkDestroyImage(device, imageContext.image, nullptr);
            vkFreeMemory(device, imageContext.imageMemory, nullptr);
        }
    }
}
