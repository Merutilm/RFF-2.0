//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <memory>

#include "Core.hpp"
#include "../repo/Repositories.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
#include "SyncObject.hpp"
#include "../context/RenderContext.hpp"

namespace merutilm::vkh {
    class Engine final : public Handler {
        Core core = nullptr;
        std::unique_ptr<Repositories> repositories = nullptr;
        std::unique_ptr<CommandPool> commandPool = nullptr;
        std::unique_ptr<CommandBuffer> commandBuffer = nullptr;
        std::unique_ptr<SyncObject> syncObject = nullptr;
        std::unique_ptr<RenderContext> renderContext = nullptr;

    public:
        explicit Engine(Core &&core);

        ~Engine() override;

        Engine(const Engine &) = delete;

        Engine &operator=(const Engine &) = delete;

        Engine(Engine &&) = delete;

        Engine &operator=(Engine &&) = delete;

        std::unique_ptr<RenderContext> attachRenderContext(std::unique_ptr<RenderContext> &&renderContext);

        [[nodiscard]] CoreRef getCore() const { return *core; }

        [[nodiscard]] Repositories &getRepositories() const { return *repositories; }

        [[nodiscard]] CommandPool &getCommandPool() const { return *commandPool; }

        [[nodiscard]] CommandBuffer &getCommandBuffer() const { return *commandBuffer; }

        [[nodiscard]] SyncObject &getSyncObject() const { return *syncObject; }

        [[nodiscard]] RenderContext &getRenderContext() const { return *renderContext; }

    private:
        void init() override;

        void destroy() override;
    };
}
