//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Sampler.hpp"
#include <vulkan_helper/engine/context/ImageContext.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class CombinedImageSampler final : public CoreHandler {
        std::variant<MultiframeImageContext, ImageContext> imageContext = {};
        Sampler& sampler;
        bool multiframeEnabled = false;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit CombinedImageSampler(Core & core, Sampler & sampler, bool multiframeEnabled);

        ~CombinedImageSampler() override;

        CombinedImageSampler(const CombinedImageSampler &) = delete;

        CombinedImageSampler &operator=(const CombinedImageSampler &) = delete;

        CombinedImageSampler(CombinedImageSampler &&) = delete;

        CombinedImageSampler &operator=(CombinedImageSampler &&) = delete;

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

        [[nodiscard]] Sampler & getSampler() const { return sampler; }

    private:
        void init() override;

        void cleanup() override;
    };


}
