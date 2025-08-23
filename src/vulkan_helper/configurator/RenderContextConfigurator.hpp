//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include <ranges>

#include "../manage/RenderPassManager.hpp"

namespace merutilm::vkh {
    /**
     * RenderPass Framebuffer Configurator
     */
    struct RenderContextConfigurator {
        CoreRef core;
        SwapchainRef swapchain;
        std::vector<MultiframeImageContext> contexts = {};

        explicit RenderContextConfigurator(CoreRef core, SwapchainRef swapchain) : core(core), swapchain(swapchain) {
        }

        virtual ~RenderContextConfigurator() {
            cleanupContexts();
        }

        RenderContextConfigurator(const RenderContextConfigurator &) = delete;

        RenderContextConfigurator &operator=(const RenderContextConfigurator &) = delete;

        RenderContextConfigurator(RenderContextConfigurator &&) = delete;

        RenderContextConfigurator &operator=(RenderContextConfigurator &&) = delete;

        void configure(const RenderPassManager &manager) {
            configureImageContext();
            configureRenderContext(manager);
        }

        virtual void configureImageContext() = 0;

        virtual void configureRenderContext(const RenderPassManager &manager) = 0;


        void appendStoredImageContext(const uint32_t contextIndexExpected, MultiframeImageContext &&context) {
            contexts.emplace_back(std::move(context));
            SafeArrayChecker::checkIndexEqual(contextIndexExpected, contexts.size() - 1, "Stored Image Context");
        }

        const ImageContext &getImageContext(const uint32_t contextIndex, const uint32_t imageIndex) const {
            return contexts.at(contextIndex)[imageIndex];
        }

        void cleanupContexts() {
            for (const auto &context : contexts) {
                ImageContext::destroyContext(core, &context);
            }
            contexts.clear();
        }


    };
}
