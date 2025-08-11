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
        const Core &core;
        const Swapchain &swapchain;
        std::vector<MultiframeImageContext> contexts = {};

        explicit RenderContextConfigurator(const Core &core, const Swapchain &swapchain) : core(core), swapchain(swapchain) {
        }

        virtual ~RenderContextConfigurator() {
            cleanupContexts();
        }

        RenderContextConfigurator(const RenderContextConfigurator &) = delete;

        RenderContextConfigurator &operator=(const RenderContextConfigurator &) = delete;

        RenderContextConfigurator(RenderContextConfigurator &&) = delete;

        RenderContextConfigurator &operator=(RenderContextConfigurator &&) = delete;

        void configure(RenderPassManager &manager) {
            configureImageContext();
            configureRenderContext(manager);
        }

        virtual void configureImageContext() = 0;

        virtual void configureRenderContext(RenderPassManager &manager) = 0;


        void appendStoredImageContext(const uint32_t contextIndexExpected, MultiframeImageContext &&context) {
            contexts.emplace_back(std::move(context));
            IndexChecker::checkIndexEqual(contextIndexExpected, contexts.size() - 1, "Stored Image Context");
        }

        const MultiframeImageContext &getImageContext(const uint32_t contextIndex) const {
            return contexts.at(contextIndex);
        }

        void cleanupContexts() {
            for (const auto &context : contexts) {
                ImageContext::destroy(core, context);
            }
            contexts.clear();
        }


    };
}
