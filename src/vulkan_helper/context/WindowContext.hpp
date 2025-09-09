//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "RenderContext.hpp"
#include "../core/vkh_base.hpp"
#include "../impl/CommandBuffer.hpp"

#include "../impl/GraphicsContextWindow.hpp"
#include "../impl/Surface.hpp"
#include "../impl/Swapchain.hpp"
#include "../impl/SyncObject.hpp"
#include "../repo/Repositories.hpp"

namespace merutilm::vkh {
    class WindowContextImpl final : public CoreHandler{
        const uint32_t attachmentIndex;
        GraphicsContextWindow window = nullptr;
        Surface surface = nullptr;
        Swapchain swapchain = nullptr;
        Repositories windowLocalRepositories = nullptr;
        CommandPool commandPool = nullptr;
        CommandBuffer commandBuffer = nullptr;
        SyncObject syncObject = nullptr;
        SharedImageContext sharedImageContext = nullptr;
        std::vector<RenderContext> renderContext = {};
    public:
        explicit WindowContextImpl(CoreRef core, uint32_t index, GraphicsContextWindow&& window);

        ~WindowContextImpl() override;

        template<typename T, typename ExtentImgGetter, typename SwapchainImgGetter> requires (
            std::is_base_of_v<RenderContextConfiguratorAbstract, T> && std::is_invocable_r_v<VkExtent2D, ExtentImgGetter> && std::is_invocable_r_v<MultiframeImageContext, SwapchainImgGetter>)
        void attachRenderContext(CoreRef core, ExtentImgGetter &&extentGetter,
                                 SwapchainImgGetter &&swapchainImageContext) {
            safe_array::check_index_equal(T::CONTEXT_INDEX, static_cast<uint32_t>(this->renderContext.size()),
                                              "Render Context Index");
            this->renderContext.emplace_back(
                factory::create<RenderContext>(core, std::forward<ExtentImgGetter>(extentGetter),
                                               std::make_unique<T>(core, *sharedImageContext, std::forward<SwapchainImgGetter>(swapchainImageContext))));
        }



        template<typename Configurator> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Configurator>
        [[nodiscard]] Configurator & getRenderContextConfigurator() {
            return *dynamic_cast<Configurator *>(getRenderContext(Configurator::CONTEXT_INDEX).getConfigurator());
        }

        [[nodiscard]] uint32_t getAttachmentIndex() const {
            return attachmentIndex;
        }

        [[nodiscard]] GraphicsContextWindowRef getWindow() const {
            return *window;
        }

        [[nodiscard]] SurfaceRef getSurface() const {
            return *surface;
        }

        [[nodiscard]] SwapchainRef getSwapchain() const {
            return *swapchain;
        }

        [[nodiscard]] RepositoriesRef getWindowLocalRepositories() const {
            return *windowLocalRepositories;
        }

        [[nodiscard]] CommandPoolRef getCommandPool() const {
            return *commandPool;
        }

        [[nodiscard]] CommandBufferRef getCommandBuffer() const {
            return *commandBuffer;
        }

        [[nodiscard]] SyncObjectRef getSyncObject() const {
            return *syncObject;
        }

        [[nodiscard]] SharedImageContextRef getSharedImageContext() const {
            return *sharedImageContext;
        }

        [[nodiscard]] std::span<const RenderContext> getRenderContexts() const { return renderContext; }

        [[nodiscard]] RenderContextRef getRenderContext(const uint32_t renderContextIndex) const {
            return *renderContext[renderContextIndex];
        }

        void init() override;

        void configureRepositories() const;

        void destroy() override;
    };
    using WindowContext = std::unique_ptr<WindowContextImpl>;
    using WindowContextPtr = WindowContextImpl *;
    using WindowContextRef = WindowContextImpl &;
}
