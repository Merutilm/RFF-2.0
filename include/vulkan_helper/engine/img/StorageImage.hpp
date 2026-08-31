//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include <vulkan_helper/engine/context/ImageContext.hpp>

namespace merutilm::vkh {
    class StorageImage : public CoreHandler{
        std::variant<MultiframeImageContext, ImageContext> imageContext = {};
        bool multiframeEnabled = false;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit StorageImage(Core & core, bool multiframeEnabled);

        ~StorageImage() override;

        StorageImage(const StorageImage &) = delete;

        StorageImage &operator=(const StorageImage &) = delete;

        StorageImage(StorageImage &&) = delete;

        StorageImage &operator=(StorageImage &&) = delete;

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

    protected:
        void init() override;

        void cleanup() override;
    };
}
