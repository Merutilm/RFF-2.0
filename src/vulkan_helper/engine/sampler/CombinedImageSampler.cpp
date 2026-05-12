//
// Created by Merutilm on 2025-07-18.
//

#include <vulkan_helper/engine/sampler/CombinedImageSampler.hpp>

#include <vulkan_helper/util/BufferImageContextUtils.hpp>

namespace merutilm::vkh {
    CombinedImageSampler::CombinedImageSampler(Core & core, Sampler & sampler,
                                                       const bool multiframeEnabled) : CoreHandler(core),
        sampler(sampler), multiframeEnabled(multiframeEnabled) {
        CombinedImageSampler::init();
    }


    CombinedImageSampler::~CombinedImageSampler() {
        CombinedImageSampler::cleanup();
    }


    const ImageContext &CombinedImageSampler::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (multiframeEnabled) {
            throw exception_invalid_state("Sampler is multiframed (const)");
        }
        return std::get<ImageContext>(imageContext);
    }

    const MultiframeImageContext &CombinedImageSampler::getImageContextMF() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (!multiframeEnabled) {
            throw exception_invalid_state("Sampler is not multiframe (const)");
        }
        return std::get<MultiframeImageContext>(imageContext);
    }

    ImageContext &CombinedImageSampler::getImageContext() {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (multiframeEnabled) {
            throw exception_invalid_state("Sampler is multiframed");
        }
        return std::get<ImageContext>(imageContext);
    }

    MultiframeImageContext &CombinedImageSampler::getImageContextMF() {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (!multiframeEnabled) {
            throw exception_invalid_state("Sampler is not multiframe");
        }
        return std::get<MultiframeImageContext>(imageContext);
    }

    void CombinedImageSampler::setImageContext(const ImageContext &imageContext) {
        if (multiframeEnabled) {
            throw exception_invalid_state("Sampler is multiframed");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContext());
        }
        initialized = true;
        isUnique = false;
        this->imageContext = imageContext;
    }

    void CombinedImageSampler::setUniqueImageContext(const ImageContext &imageContext) {
        if (multiframeEnabled) {
            throw exception_invalid_state("Sampler is multiframed (Unique)");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContext());
        }

        initialized = true;
        isUnique = true;
        this->imageContext = imageContext;
    }

    void CombinedImageSampler::setImageContextMF(const MultiframeImageContext &imageContext) {
        if (!multiframeEnabled) {
            throw exception_invalid_state("Sampler is not multiframe");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContextMF());
        }
        initialized = true;
        isUnique = false;
        this->imageContext = imageContext;
    }

    void CombinedImageSampler::setUniqueImageContextMF(const MultiframeImageContext &imageContext) {
        if (!multiframeEnabled) {
            throw exception_invalid_state("Sampler is not multiframe (Unique)");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContextMF());
        }
        initialized = true;
        isUnique = true;
        this->imageContext = imageContext;
    }


    void CombinedImageSampler::init() {
        //no operation
    }

    void CombinedImageSampler::cleanup() {
        if (isUnique) {
            if (multiframeEnabled) {
                ImageContext::destroyContext(core, getImageContextMF());
            } else {
                ImageContext::destroyContext(core, getImageContext());
            }
        }
    }
}
