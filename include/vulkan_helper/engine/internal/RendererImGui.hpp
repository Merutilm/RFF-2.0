//
// Created by Merutilm on 7/7/26.
//

#pragma once
#ifdef VKH_USE_IMGUI
#include "vulkan_helper/engine/graphics/Renderer.hpp"
namespace merutilm::vkh {
    struct RendererImGui : Renderer {
        RenderContext *imguiRenderContext = nullptr;
        std::function<void()> renderFunc;

        template<typename F>
            requires std::is_invocable_v<F>
        explicit RendererImGui(Engine &engine, WindowContext &wc, F &&renderFunc) :
            Renderer(engine, wc), renderFunc(std::forward<F>(renderFunc)) {
            RendererImGui::init();
        }

        ~RendererImGui() override { RendererImGui::cleanup(); };
        RendererImGui(const RendererImGui &) = delete;
        RendererImGui &operator=(const RendererImGui &) = delete;
        RendererImGui(RendererImGui &&) = delete;
        RendererImGui &operator=(RendererImGui &&) = delete;

    protected:
        void init() override;
        void cleanup() override;
        void beforeCmdRender() override;
        void cmdRender(uint32_t swapchainImageIndex) override;
    };
} // namespace merutilm::vkh
#endif