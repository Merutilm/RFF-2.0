//
// Created by Merutilm on 2025-09-03.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/context/ImageContext.hpp>

namespace merutilm::vkh {
    class SharedImageContext final {
        Core & core;
        std::vector<MultiframeImageContext> multiframeContexts = {};
        std::vector<ImageContext> imageContexts = {};

    public:
        explicit SharedImageContext(Core & core);

        ~SharedImageContext();

        SharedImageContext(const SharedImageContext &) = delete;

        SharedImageContext &operator=(const SharedImageContext &) = delete;

        SharedImageContext(SharedImageContext &&) = delete;

        SharedImageContext &operator=(SharedImageContext &&) = delete;

        void appendMultiframeImageContext(uint32_t expected, ImageInitInfo &&iii);

        void appendImageContext(uint32_t expected, ImageInitInfo &&iii);

        MultiframeImageContext &getImageContextMF(const uint32_t index) {
            return multiframeContexts[index];
        }

        ImageContext &getImageContext(const uint32_t index) {
            return imageContexts[index];
        }

        void cleanupContexts();
    };


}
