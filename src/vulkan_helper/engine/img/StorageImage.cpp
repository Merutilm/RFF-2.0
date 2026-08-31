//
// Created by Merutilm on 8/31/26.
//

#include <vulkan_helper/engine/img/StorageImage.hpp>

#include <vulkan_helper/util/BufferImageContextUtils.hpp>

namespace merutilm::vkh {
    StorageImage::StorageImage(Core & core, const bool multiframeEnabled) : CoreHandler(core), multiframeEnabled(multiframeEnabled) {
        StorageImage::init();
    }

    StorageImage::~StorageImage() {
        StorageImage::cleanup();
    }

    const ImageContext &StorageImage::getImageContext() const {
        if (!initialized) {
            throw exception_invalid_state{"StorageImage2D is not initialized. Is setImageContext() called?"};
        }
        if (multiframeEnabled) {
            throw exception_invalid_state("StorageImage is multiframed (const)");
        }
        return std::get<ImageContext>(imageContext);
    }

    const MultiframeImageContext &StorageImage::getImageContextMF() const {
        if (!initialized) {
            throw exception_invalid_state{"StorageImage2D is not initialized. Is setImageContext() called?"};
        }
        if (!multiframeEnabled) {
            throw exception_invalid_state("StorageImage is not multiframe (const)");
        }
        return std::get<MultiframeImageContext>(imageContext);
    }

    ImageContext &StorageImage::getImageContext() {
        if (!initialized) {
            throw exception_invalid_state{"StorageImage2D is not initialized. Is setImageContext() called?"};
        }
        if (multiframeEnabled) {
            throw exception_invalid_state("StorageImage is multiframed");
        }
        return std::get<ImageContext>(imageContext);
    }

    MultiframeImageContext &StorageImage::getImageContextMF() {
        if (!initialized) {
            throw exception_invalid_state{"StorageImage2D is not initialized. Is setImageContext() called?"};
        }
        if (!multiframeEnabled) {
            throw exception_invalid_state("StorageImage is not multiframe");
        }
        return std::get<MultiframeImageContext>(imageContext);
    }

    void StorageImage::setImageContext(const ImageContext &imageContext) {
        if (multiframeEnabled) {
            throw exception_invalid_state("StorageImage is multiframed");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContext());
        }
        initialized = true;
        isUnique = false;
        this->imageContext = imageContext;
    }

    void StorageImage::setUniqueImageContext(const ImageContext &imageContext) {
        if (multiframeEnabled) {
            throw exception_invalid_state("StorageImage is multiframed (Unique)");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContext());
        }

        initialized = true;
        isUnique = true;
        this->imageContext = imageContext;
    }

    void StorageImage::setImageContextMF(const MultiframeImageContext &imageContext) {
        if (!multiframeEnabled) {
            throw exception_invalid_state("StorageImage is not multiframe");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContextMF());
        }
        initialized = true;
        isUnique = false;
        this->imageContext = imageContext;
    }

    void StorageImage::setUniqueImageContextMF(const MultiframeImageContext &imageContext) {
        if (!multiframeEnabled) {
            throw exception_invalid_state("StorageImage is not multiframe (Unique)");
        }
        if (isUnique) {
            ImageContext::destroyContext(core, getImageContextMF());
        }
        initialized = true;
        isUnique = true;
        this->imageContext = imageContext;
    }


    void StorageImage::init() {
        //no operation
    }

    void StorageImage::cleanup() {
        if (isUnique) {
            if (multiframeEnabled) {
                ImageContext::destroyContext(core, getImageContextMF());
            } else {
                ImageContext::destroyContext(core, getImageContext());
            }
        }
    }
}
