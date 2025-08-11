//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "../context/ImageContext.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class Sampler2D final : public CoreHandler {
        ImageContext imageContext;
        VkSampler sampler = nullptr;
        VkSamplerCreateInfo samplerInfo;
        bool initialized = false;
        bool isUnique = false;

    public:
        explicit Sampler2D(const Core &core, VkSamplerCreateInfo &&samplerInfo);

        ~Sampler2D() override;

        Sampler2D(const Sampler2D &) = delete;

        Sampler2D &operator=(const Sampler2D &) = delete;

        Sampler2D(Sampler2D &&) = delete;

        Sampler2D &operator=(Sampler2D &&) = delete;

        void setImageContext(const ImageContext &imageContext) {
            initialized = true;
            this->imageContext = imageContext;
        }

        void setImageContext(ImageContext &&imageContext) {
            initialized = true;
            isUnique = true;
            this->imageContext = std::move(imageContext);
        }

        [[nodiscard]] const ImageContext &getImageContext() const;

        [[nodiscard]] bool isInitialized() const { return initialized; }

        [[nodiscard]] VkSampler getSamplerHandle() const { return sampler; }

    private:
        void init() override;

        void destroy() override;
    };
}
