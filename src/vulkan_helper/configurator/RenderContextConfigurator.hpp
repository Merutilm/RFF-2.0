//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include "../context/ImageContext.hpp"
#include "../impl/CommandPool.hpp"
#include "../manage/RenderPassManager.hpp"

namespace merutilm::vkh {
    /**
     * RenderPass Framebuffer Configurator
     */
    struct RenderContextConfiguratorAbstract {
        CoreRef core;
        CommandPoolRef commandPool;
        std::function<MultiframeImageContext()> swapchainImageContextGetter;
        std::vector<MultiframeImageContext> contexts = {};

        template<typename F> requires std::is_invocable_r_v<MultiframeImageContext, F>
        explicit RenderContextConfiguratorAbstract(CoreRef core, CommandPoolRef commandPool, const F &swapchainImageContextGetter) : core(core), commandPool(commandPool),
            swapchainImageContextGetter(swapchainImageContextGetter) {
        }

        virtual ~RenderContextConfiguratorAbstract() {
            cleanupContexts();
        }

        RenderContextConfiguratorAbstract(const RenderContextConfiguratorAbstract &) = delete;

        RenderContextConfiguratorAbstract &operator=(const RenderContextConfiguratorAbstract &) = delete;

        RenderContextConfiguratorAbstract(RenderContextConfiguratorAbstract &&) = delete;

        RenderContextConfiguratorAbstract &operator=(RenderContextConfiguratorAbstract &&) = delete;

        void configure(const VkExtent2D &extent, RenderPassManagerRef manager) {
            configureImageContext(extent);
            configureRenderContext(manager);
        }

        virtual void configureImageContext(const VkExtent2D &extent) = 0;

        virtual void configureRenderContext(RenderPassManagerRef manager) = 0;


        void appendStoredImageContext(const uint32_t contextIndexExpected, MultiframeImageContext &&context) {
            SafeArrayChecker::checkIndexEqual(contextIndexExpected, static_cast<uint32_t>(contexts.size()),
                                              "Stored Image Context");
            contexts.emplace_back(std::move(context));
        }

        [[nodiscard]] const MultiframeImageContext &getImageContext(const uint32_t contextIndex) const {
            return contexts.at(contextIndex);
        }

        void cleanupContexts() {
            for (const auto &context: contexts) {
                ImageContext::destroyContext(core, context);
            }
            contexts.clear();
        }
    };


    using RenderContextConfigurator = std::unique_ptr<RenderContextConfiguratorAbstract>;
    using RenderContextConfiguratorPtr = RenderContextConfiguratorAbstract *;
    using RenderContextConfiguratorRef = RenderContextConfiguratorAbstract &;
}
