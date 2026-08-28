//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/cmd/CommandBuffer.hpp>

#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    CommandBuffer::CommandBuffer(Core & core, CommandPool & commandPool) : CoreHandler(core), commandPool(commandPool) {
        CommandBuffer::init();
    }

    CommandBuffer::~CommandBuffer() {
        CommandBuffer::cleanup();
    }

    void CommandBuffer::begin() const {
        constexpr VkCommandBufferBeginInfo beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                        .pNext = nullptr,
                                                        .flags = 0,
                                                        .pInheritanceInfo = nullptr};
        vkResetCommandBuffer(commandBuffer, 0);
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw exception_init("Failed to begin command buffer operation.");
        }
    }
    void CommandBuffer::end() const { vkEndCommandBuffer(commandBuffer); }
    void CommandBuffer::submit(const Fence *const fence, const std::vector<const Semaphore *> &waitSemaphores,
                               const std::vector<const Semaphore *> &signalSemaphores) const {

        std::vector<VkSemaphore> waitSemaphoreHandles(waitSemaphores.size());
        std::vector<VkPipelineStageFlags> waitDstStageFlags(waitSemaphores.size());
        std::vector<VkSemaphore> signalSemaphoreHandles(signalSemaphores.size());

        std::ranges::transform(waitSemaphores, waitSemaphoreHandles.begin(),
                               [](const Semaphore *semaphore) { return semaphore->getSemaphoreHandle(); });
        std::ranges::transform(waitSemaphores, waitDstStageFlags.begin(),
                               [](const Semaphore *semaphore) { return semaphore->getWaitStageMask(); });
        std::ranges::transform(signalSemaphores, signalSemaphoreHandles.begin(),
                               [](const Semaphore *semaphore) { return semaphore->getSemaphoreHandle(); });

        const VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = nullptr,
                .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphoreHandles.size()),
                .pWaitSemaphores = waitSemaphoreHandles.empty() ? nullptr : waitSemaphoreHandles.data(),
                .pWaitDstStageMask = waitSemaphoreHandles.empty() ? nullptr : waitDstStageFlags.data(),
                .commandBufferCount = 1,
                .pCommandBuffers = &commandBuffer,
                .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphoreHandles.size()),
                .pSignalSemaphores = signalSemaphoreHandles.empty() ? nullptr : signalSemaphoreHandles.data()};

        core.getLogicalDevice().queueSubmit(1, &submitInfo, fence ? fence->getFenceHandle() : nullptr);
    }
    void CommandBuffer::init() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkCommandBufferAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool.getCommandPoolHandle(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw exception_init("Failed to allocate command buffers!");
        }
    }

    void CommandBuffer::cleanup() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkFreeCommandBuffers(device, commandPool.getCommandPoolHandle(), 1, &commandBuffer);

    }



}
