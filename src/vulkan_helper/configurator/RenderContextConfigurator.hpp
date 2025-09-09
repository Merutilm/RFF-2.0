//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include "../context/SharedImageContext.hpp"
#include "../context/ImageContext.hpp"
#include "../manage/RenderPassManager.hpp"

namespace merutilm::vkh {
    /**
     * RenderPass Framebuffer Configurator
     */
    struct RenderContextConfiguratorAbstract {
        CoreRef core;
        SharedImageContextRef sharedImageContext;
        std::function<MultiframeImageContext()> swapchainImageContextGetter;
        bool firstFrame = true;

        template<typename F> requires std::is_invocable_r_v<MultiframeImageContext, F>
        explicit RenderContextConfiguratorAbstract(CoreRef core, SharedImageContextRef sharedImageContext, const F &swapchainImageContextGetter) : core(core), sharedImageContext(sharedImageContext),
            swapchainImageContextGetter(swapchainImageContextGetter) {
        }

        virtual ~RenderContextConfiguratorAbstract() = default;

        RenderContextConfiguratorAbstract(const RenderContextConfiguratorAbstract &) = delete;

        RenderContextConfiguratorAbstract &operator=(const RenderContextConfiguratorAbstract &) = delete;

        RenderContextConfiguratorAbstract(RenderContextConfiguratorAbstract &&) = delete;

        RenderContextConfiguratorAbstract &operator=(RenderContextConfiguratorAbstract &&) = delete;

        virtual void configure(RenderPassManagerRef rpm) = 0;

        void allFrameInitialized() {
            firstFrame = false;
        }

    };


    using RenderContextConfigurator = std::unique_ptr<RenderContextConfiguratorAbstract>;
    using RenderContextConfiguratorPtr = RenderContextConfiguratorAbstract *;
    using RenderContextConfiguratorRef = RenderContextConfiguratorAbstract &;
}
