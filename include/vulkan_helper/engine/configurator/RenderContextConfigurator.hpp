//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include <vulkan_helper/engine/context/SharedImageContext.hpp>
#include <vulkan_helper/engine/context/ImageContext.hpp>
#include <vulkan_helper/engine/manage/RenderPassManager.hpp>

namespace merutilm::vkh {
    /**
     * RenderPass Framebuffer Configurator
     */
    struct RenderContextConfigurator {
        Core & core;
        SharedImageContext & sharedImageContext;
        std::function<MultiframeImageContext()> swapchainImageContextGetter;
        bool firstFrame = true;

        template<typename F> requires std::is_invocable_r_v<MultiframeImageContext, F>
        explicit RenderContextConfigurator(Core & core, SharedImageContext & sharedImageContext, const F &swapchainImageContextGetter) : core(core), sharedImageContext(sharedImageContext),
            swapchainImageContextGetter(swapchainImageContextGetter) {
        }

        virtual ~RenderContextConfigurator() = default;

        RenderContextConfigurator(const RenderContextConfigurator &) = delete;

        RenderContextConfigurator &operator=(const RenderContextConfigurator &) = delete;

        RenderContextConfigurator(RenderContextConfigurator &&) = delete;

        RenderContextConfigurator &operator=(RenderContextConfigurator &&) = delete;

        virtual void configure(RenderPassManager & rpm) = 0;

        void allFrameInitialized() {
            firstFrame = false;
        }

    };



}
