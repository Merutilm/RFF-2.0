//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Sampler.hpp"
#include "../context/ImageContext.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class CombinedImageSampler final : public CoreHandler {
        ImageContext imageContext = {};
        const Sampler &sampler;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit CombinedImageSampler(const Core &core, const Sampler &sampler);

        ~CombinedImageSampler() override;

        CombinedImageSampler(const CombinedImageSampler &) = delete;

        CombinedImageSampler &operator=(const CombinedImageSampler &) = delete;

        CombinedImageSampler(CombinedImageSampler &&) = delete;

        CombinedImageSampler &operator=(CombinedImageSampler &&) = delete;

        void setImageContext(const ImageContext &imageContext) {
            if (isUnique) {
                ImageContext::destroyContext(core, &this->imageContext);
            }
            initialized = true;
            isUnique = false;
            this->imageContext = imageContext;
        }

        void setUniqueImageContext(ImageContext &&imageContext) {
            if (isUnique) {
                ImageContext::destroyContext(core, &this->imageContext);
            }
            initialized = true;
            isUnique = true;
            this->imageContext = std::move(imageContext);
        }

        [[nodiscard]] const ImageContext &getImageContext() const;

        [[nodiscard]] bool isInitialized() const { return initialized; }

        [[nodiscard]] const Sampler &getSampler() const { return sampler; }

    private:
        void init() override;

        void destroy() override;
    };
}
