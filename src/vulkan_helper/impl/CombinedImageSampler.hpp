//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Sampler.hpp"
#include "../context/ImageContext.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class CombinedImageSamplerImpl final : public CoreHandler {
        std::variant<MultiframeImageContext, ImageContext> imageContext = {};
        SamplerRef sampler;
        bool multiframeEnabled = false;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit CombinedImageSamplerImpl(CoreRef core, SamplerRef sampler, bool multiframeEnabled);

        ~CombinedImageSamplerImpl() override;

        CombinedImageSamplerImpl(const CombinedImageSamplerImpl &) = delete;

        CombinedImageSamplerImpl &operator=(const CombinedImageSamplerImpl &) = delete;

        CombinedImageSamplerImpl(CombinedImageSamplerImpl &&) = delete;

        CombinedImageSamplerImpl &operator=(CombinedImageSamplerImpl &&) = delete;

        void setImageContext(const ImageContext &imageContext);

        void setUniqueImageContext(const ImageContext &imageContext);

        void setImageContextMF(const MultiframeImageContext &imageContext);

        void setUniqueImageContextMF(const MultiframeImageContext &imageContext);

        [[nodiscard]] const ImageContext &getImageContext() const;

        [[nodiscard]] const MultiframeImageContext &getImageContextMF() const;

        [[nodiscard]] ImageContext &getImageContext();

        [[nodiscard]] MultiframeImageContext &getImageContextMF();

        [[nodiscard]] bool isMultiframe() const {return multiframeEnabled;}

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
