//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "Sampler.hpp"
#include "../context/ImageContext.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class CombinedMultiframeImageSamplerImpl final : public CoreHandler {
        MultiframeImageContext imageContext = {};
        SamplerRef sampler;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit CombinedMultiframeImageSamplerImpl(CoreRef core, SamplerRef sampler);

        ~CombinedMultiframeImageSamplerImpl() override;

        CombinedMultiframeImageSamplerImpl(const CombinedMultiframeImageSamplerImpl &) = delete;

        CombinedMultiframeImageSamplerImpl &operator=(const CombinedMultiframeImageSamplerImpl &) = delete;

        CombinedMultiframeImageSamplerImpl(CombinedMultiframeImageSamplerImpl &&) = delete;

        CombinedMultiframeImageSamplerImpl &operator=(CombinedMultiframeImageSamplerImpl &&) = delete;

        void setImageContext(const MultiframeImageContext &imageContext) {
            if (isUnique) {
                ImageContext::destroyContext(core, this->imageContext);
            }
            initialized = true;
            isUnique = false;
            this->imageContext = imageContext;
        }

        void setUniqueImageContext(MultiframeImageContext &&imageContext) {
            if (isUnique) {
                ImageContext::destroyContext(core, this->imageContext);
            }
            initialized = true;
            isUnique = true;
            this->imageContext = std::move(imageContext);
        }

        [[nodiscard]] const MultiframeImageContext &getImageContext() const;

        [[nodiscard]] bool isInitialized() const { return initialized; }

        [[nodiscard]] SamplerRef getSampler() const { return sampler; }

    private:
        void init() override;

        void destroy() override;
    };

    using CombinedMultiframeImageSampler = std::unique_ptr<CombinedMultiframeImageSamplerImpl>;
    using CombinedMultiframeImageSamplerPtr = CombinedMultiframeImageSamplerImpl *;
    using CombinedMultiframeImageSamplerRef = CombinedMultiframeImageSamplerImpl &;
}
