//
// Created by Merutilm on 2025-08-28.
//

#include <vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp>

#include "vulkan_helper/engine/cmd/CommandBuffer.hpp"

namespace merutilm::vkh {


    ScopedCommandBufferExecutor::ScopedCommandBufferExecutor(WindowContext &wc, const CommandBuffer &commandBuffer,
                                                             const Fence &fence, const Semaphore *wait, const Semaphore *signal) :
        WindowContextHandler(wc), commandBuffer(commandBuffer), fence(fence), wait(wait), signal(signal) {
        ScopedCommandBufferExecutor::init();
    }
    ScopedCommandBufferExecutor::~ScopedCommandBufferExecutor() { ScopedCommandBufferExecutor::cleanup(); }


    void ScopedCommandBufferExecutor::init() {
        commandBuffer.begin();
    }

    void ScopedCommandBufferExecutor::cleanup() {
        static const std::vector<const Semaphore *> V{};
        commandBuffer.end();
        commandBuffer.submit(&fence, wait ? std::vector{wait} : V, signal ? std::vector{signal} : V);
    }
} // namespace merutilm::vkh
