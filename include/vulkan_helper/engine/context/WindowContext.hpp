//
// Created by Merutilm on 2025-07-18.
//

#pragma once

#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/window/Swapchain.hpp>
#include <vulkan_helper/engine/window/Surface.hpp>
#include <vulkan_helper/engine/cmd/CommandBuffer.hpp>
#include <vulkan_helper/engine/repo/Repositories.hpp>
#include <vulkan_helper/engine/sync/SyncObject.hpp>
#include "RenderContext.hpp"

namespace merutilm::vkh {
    class WindowContext final : public CoreHandler {
        const uint32_t attachmentIndex;
        std::unique_ptr<PlatformWindowBase> window;
        std::optional<Surface> surface;
        std::optional<Swapchain> swapchain;
        std::optional<Repositories> windowLocalRepositories;
        std::optional<CommandPool> commandPool;
        std::optional<CommandBuffer> commandBuffer;
        std::optional<SyncObject> syncObject;
        std::optional<SharedImageContext> sharedImageContext;
        std::vector<std::unique_ptr<RenderContext>> renderContexts = {};

    public:
        explicit WindowContext(Core &core, uint32_t index, std::unique_ptr<PlatformWindowBase> &&window);

        ~WindowContext() override;

        WindowContext(const WindowContext &) = delete;

        WindowContext operator=(const WindowContext &) = delete;

        WindowContext(WindowContext &&) = delete;

        WindowContext operator=(WindowContext &&) = delete;

        template<typename T, typename ExtentImgGetter, typename SwapchainImgGetter>
            requires(std::is_base_of_v<RenderContextConfigurator, T> &&
                     std::is_invocable_r_v<VkExtent2D, ExtentImgGetter> &&
                     std::is_invocable_r_v<MultiframeImageContext, SwapchainImgGetter>)
        void attachRenderContext(ExtentImgGetter &&extentGetter,
                                 SwapchainImgGetter &&swapchainImageGetter) {
            safe_array::check_index_equal(T::CONTEXT_INDEX, static_cast<uint32_t>(this->renderContexts.size()),
                                          "Render Context Index");
            this->renderContexts.emplace_back(std::make_unique<RenderContext>(
                    core, std::forward<ExtentImgGetter>(extentGetter),
                    std::make_unique<T>(core, *sharedImageContext,
                                        std::forward<SwapchainImgGetter>(swapchainImageGetter))));
        }


        template<typename Configurator>
            requires std::is_base_of_v<RenderContextConfigurator, Configurator>
        [[nodiscard]] Configurator &getRenderContextConfigurator() {
            return *dynamic_cast<Configurator *>(getRenderContext(Configurator::CONTEXT_INDEX).getConfigurator());
        }

        [[nodiscard]] uint32_t getAttachmentIndex() const { return attachmentIndex; }

        [[nodiscard]] PlatformWindowBase *getWindow() const { return window.get(); }

        [[nodiscard]] Surface &getSurface() { return *surface; }

        [[nodiscard]] Swapchain &getSwapchain() { return *swapchain; }

        [[nodiscard]] Repositories &getWindowLocalRepositories() { return *windowLocalRepositories; }

        [[nodiscard]] CommandPool &getCommandPool() { return *commandPool; }

        [[nodiscard]] CommandBuffer &getCommandBuffer() { return *commandBuffer; }

        [[nodiscard]] SyncObject &getSyncObject() { return *syncObject; }

        [[nodiscard]] SharedImageContext &getSharedImageContext() { return *sharedImageContext; }

        [[nodiscard]] std::span<const std::unique_ptr<RenderContext>> getRenderContexts() const { return renderContexts; }

        [[nodiscard]] RenderContext &getRenderContext(const uint32_t renderContextIndex) const {
            return *renderContexts[renderContextIndex];
        }

        void init() override;

        void configureRepositories();

        void cleanup() override;
    };

} // namespace merutilm::vkh
