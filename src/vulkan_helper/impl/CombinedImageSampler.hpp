//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Sampler.hpp"
#include "../context/ImageContext.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class CombinedImageSamplerImpl final : public CoreHandler {
        ImageContext imageContext = {};
        SamplerRef sampler;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit CombinedImageSamplerImpl(CoreRef core, SamplerRef sampler);

        ~CombinedImageSamplerImpl() override;

        CombinedImageSamplerImpl(const CombinedImageSamplerImpl &) = delete;

        CombinedImageSamplerImpl &operator=(const CombinedImageSamplerImpl &) = delete;

        CombinedImageSamplerImpl(CombinedImageSamplerImpl &&) = delete;

        CombinedImageSamplerImpl &operator=(CombinedImageSamplerImpl &&) = delete;

        void setImageContext(const ImageContext &imageContext) {
            if (isUnique) {
                ImageContext::destroyContext(core, &this->imageContext);
            }
            initialized = true;
            isUnique = false;
            this->imageContext = imageContext;
        }

        void setUniqueImageContext(const ImageContext &imageContext) {
            if (isUnique) {
                ImageContext::destroyContext(core, &this->imageContext);
            }
            initialized = true;
            isUnique = true;
            this->imageContext = imageContext;
        }

        [[nodiscard]] const ImageContext &getImageContext() const;

        [[nodiscard]] bool isInitialized() const { return initialized; }

        [[nodiscard]] SamplerRef getSampler() const { return sampler; }

    private:
        void init() override;

        void destroy() override;
    };

    using CombinedImageSampler = std::unique_ptr<CombinedImageSamplerImpl>;
    using CombinedImageSamplerPtr = CombinedImageSamplerImpl *;
    using CombinedImageSamplerRef = CombinedImageSamplerImpl &;
}
