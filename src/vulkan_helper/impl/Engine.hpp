//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "../core/vkh_base.hpp"

#include "Core.hpp"
#include "../repo/Repositories.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
#include "SyncObject.hpp"
#include "../context/RenderContext.hpp"

namespace merutilm::vkh {
    class EngineImpl final : public Handler {
        Core core = nullptr;
        Repositories repositories = nullptr;
        CommandPool commandPool = nullptr;
        CommandBuffer commandBuffer = nullptr;
        SyncObject syncObjectBetweenFrame = nullptr;
        SyncObject syncObjectBetweenRenderPass = nullptr;
        SharedImageContext sharedImageContext = nullptr;
        std::vector<RenderContext> renderContext = {};

    public:
        explicit EngineImpl(Core &&core);

        ~EngineImpl() override;

        EngineImpl(const EngineImpl &) = delete;

        EngineImpl &operator=(const EngineImpl &) = delete;

        EngineImpl(EngineImpl &&) = delete;

        EngineImpl &operator=(EngineImpl &&) = delete;

        template<typename T, typename ExtentImgGetter, typename SwapchainImgGetter> requires (
            std::is_base_of_v<RenderContextConfiguratorAbstract, T> && std::is_invocable_r_v<VkExtent2D, ExtentImgGetter> && std::is_invocable_r_v<MultiframeImageContext, SwapchainImgGetter>)
        void attachRenderContext(ExtentImgGetter &&extentGetter,
                                 SwapchainImgGetter &&swapchainImageContext) {
            safe_array::check_index_equal(T::CONTEXT_INDEX, static_cast<uint32_t>(this->renderContext.size()),
                                              "Render Context Index");
            this->renderContext.emplace_back(
                factory::create<RenderContext>(*core, std::forward<ExtentImgGetter>(extentGetter),
                                               std::make_unique<T>(*core, *sharedImageContext, std::forward<SwapchainImgGetter>(swapchainImageContext))));
        };

        [[nodiscard]] CoreRef getCore() const { return *core; }

        [[nodiscard]] RepositoriesRef getRepositories() const { return *repositories; }

        [[nodiscard]] CommandPoolRef getCommandPool() const { return *commandPool; }

        [[nodiscard]] CommandBufferRef getCommandBuffer() const { return *commandBuffer; }

        [[nodiscard]] SyncObjectRef getSyncObjectBetweenFrame() const { return *syncObjectBetweenFrame; }

        [[nodiscard]] SyncObjectRef getSyncObjectBetweenRenderPass() const { return *syncObjectBetweenRenderPass; }

        [[nodiscard]] std::span<const RenderContext> getRenderContexts() const { return renderContext; }

        [[nodiscard]] RenderContextRef getRenderContext(const uint32_t renderContextIndex) const {
            return *renderContext[renderContextIndex];
        }

        template<typename Configurator> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Configurator>
        [[nodiscard]] Configurator & getRenderContextConfigurator() {
            return *dynamic_cast<Configurator *>(getRenderContext(Configurator::CONTEXT_INDEX).getConfigurator());
        }

        SharedImageContextRef getSharedImageContext() const {
            return *sharedImageContext;
        }

    private:
        void init() override;

        void destroy() override;
    };


    using Engine = std::unique_ptr<EngineImpl>;
    using EnginePtr = EngineImpl *;
    using EngineRef = EngineImpl &;
}
