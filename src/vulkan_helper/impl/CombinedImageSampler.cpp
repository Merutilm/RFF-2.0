//
// Created by Merutilm on 2025-07-18.
//

#include "CombinedImageSampler.hpp"

#include "../util/BufferImageContextUtils.hpp"

namespace merutilm::vkh {
    CombinedImageSamplerImpl::CombinedImageSamplerImpl(CoreRef core, SamplerRef sampler,
                                                       const bool multiframeEnabled) : CoreHandler(core),
        sampler(sampler), multiframeEnabled(multiframeEnabled) {
        CombinedImageSamplerImpl::init();
    }


    CombinedImageSamplerImpl::~CombinedImageSamplerImpl() {
        CombinedImageSamplerImpl::destroy();
    }


    const ImageContext &CombinedImageSamplerImpl::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (multiframeEnabled) {
            throw exception_invalid_state("Sampler is multiframed (const)");
        }
        return std::get<ImageContext>(imageContext);
    }

    const MultiframeImageContext &CombinedImageSamplerImpl::getImageContextMF() const {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (!multiframeEnabled) {
            throw exception_invalid_state("Sampler is not multiframe (const)");
        }
        return std::get<MultiframeImageContext>(imageContext);
    }

    ImageContext &CombinedImageSamplerImpl::getImageContext() {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (multiframeEnabled) {
            throw exception_invalid_state("Sampler is multiframed");
        }
        return std::get<ImageContext>(imageContext);
    }

    MultiframeImageContext &CombinedImageSamplerImpl::getImageContextMF() {
        if (!initialized) {
            throw exception_invalid_state{"Sampler2D is not initialized. Is setImageContext() called?"};
        }
        if (!multiframeEnabled) {
            throw exception_invalid_state("Sampler is not multiframe");
        }
        return std::get<MultiframeImageContext>(imageContext);
    }

    void CombinedImageSamplerImpl::setImageContext(const ImageContext &imageContext) {
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

    void CombinedImageSamplerImpl::setUniqueImageContext(const ImageContext &imageContext) {
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

    void CombinedImageSamplerImpl::setImageContextMF(const MultiframeImageContext &imageContext) {
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

    void CombinedImageSamplerImpl::setUniqueImageContextMF(const MultiframeImageContext &imageContext) {
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


    void CombinedImageSamplerImpl::init() {
        //no operation
    }

    void CombinedImageSamplerImpl::destroy() {
        if (isUnique) {
            if (multiframeEnabled) {
                ImageContext::destroyContext(core, getImageContextMF());
            } else {
                ImageContext::destroyContext(core, getImageContext());
            }
        }
    }
}
