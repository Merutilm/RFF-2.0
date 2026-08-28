//
// Created by Merutilm on 2025-07-21.
//

#include <vulkan_helper/engine/executor/ScopedNewCommandBufferExecutor.hpp>

#include <vulkan_helper/engine/sync/Fence.hpp>
#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    ScopedNewCommandBufferExecutor::ScopedNewCommandBufferExecutor(Core & core, CommandPool & commandPool,
                                                                   Fence * const fence) : CoreHandler(core),
        commandPool(commandPool), fence(fence) {
        ScopedNewCommandBufferExecutor::init();
    }

    ScopedNewCommandBufferExecutor::~ScopedNewCommandBufferExecutor() {
        ScopedNewCommandBufferExecutor::cleanup();
    }

    void ScopedNewCommandBufferExecutor::init() {
        commandBuffer = std::make_unique<CommandBuffer>(core, commandPool);
        commandBuffer->begin();
    }

    void ScopedNewCommandBufferExecutor::cleanup() {
        std::unique_ptr<Fence> temp = nullptr;
        if (!fence) {
            temp = std::make_unique<Fence>(core);
            temp->reset();
        }


        commandBuffer->end();
        commandBuffer->submit(fence == nullptr ? temp.get() : fence, {}, {});
        if (fence == nullptr) {
            temp->wait();
        } else {
            fence->wait();
        }
        commandBuffer = nullptr;
    }
}
