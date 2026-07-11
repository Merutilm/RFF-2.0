//
// Created by Merutilm on 7/7/26.
//

#pragma once
#ifdef VKH_USE_IMGUI
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"
namespace merutilm::vkh {
    struct RCCImGui : RenderPassGraphGenerator {


        RenderPassAttachment *swapchainImageAttachment = nullptr;

        template<typename F> requires std::is_invocable_r_v<MultiframeImageContext, F>
        explicit RCCImGui(Engine &engine, WindowContext &wc, F &&swapchainImageContextGetter): RenderPassGraphGenerator(engine, wc, std::forward<F>(swapchainImageContextGetter)) {
        }

    protected:

        void configureAttachments() override;

        void configurePipelines() override;
    };
}
#endif