//
// Created by Merutilm on 2025-07-10.
//

#include "BufferObject.hpp"

#include "../executor/ScopedNewCommandBufferExecutor.hpp"
#include "../util/BufferImageUtils.hpp"
#include "../core/logger.hpp"
#include "../util/BarrierUtils.hpp"
#include "../util/BufferImageContextUtils.hpp"

namespace merutilm::vkh {
    BufferObjectAbstract::BufferObjectAbstract(CoreRef core, HostDataObjectManager &&dataManager,
                                               const VkBufferUsageFlags bufferUsage,
                                               const BufferLock bufferLock) : CoreHandler(core),
                                                                              hostDataObject(
                                                                                  factory::create<HostDataObject>(
                                                                                      std::move(dataManager))),
                                                                              bufferUsage(bufferUsage),
                                                                              bufferLock(bufferLock) {
        BufferObjectAbstract::init();
    }

    BufferObjectAbstract::~BufferObjectAbstract() {
        BufferObjectAbstract::destroy();
    }


    void BufferObjectAbstract::reloadBuffer() {
        BufferObjectAbstract::destroy();
        BufferObjectAbstract::init();
    }

    void BufferObjectAbstract::update(const uint32_t frameIndex) const {
        checkFinalizedBeforeUpdate();
        memcpy(bufferContext[frameIndex].mappedMemory, hostDataObject->data.data(), hostDataObject->getTotalSizeByte());
    }

    void BufferObjectAbstract::update(const uint32_t frameIndex, const uint32_t target) const {
        checkFinalizedBeforeUpdate();
        const uint32_t offset = hostDataObject->getOffset(target);
        const uint32_t size = hostDataObject->getSizeByte(target);
        memcpy(bufferContext[frameIndex].mappedMemory + offset, hostDataObject->data.data() + offset, size);
    }

    void BufferObjectAbstract::checkFinalizedBeforeUpdate() const {
        if (locked) {
            throw exception_invalid_state(
                "BufferObjectAbstract::update() This bufferObject is already been finalized. It cannot be modified.");
        }
    }


    void BufferObjectAbstract::init() {
        const uint32_t size = hostDataObject->getTotalSizeByte();

        VkBufferUsageFlags lockFlags = 0;

        switch (bufferLock) {
                using enum BufferLock;
            case LOCK_UNLOCK:
            case LOCK_ONLY: lockFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                break;
            case ALWAYS_MUTABLE: lockFlags = 0;
                break;
        }

        bufferContext = BufferContext::createMultiframeContext(core, {
                                                             .size = size,
                                                             .usage = bufferUsage | lockFlags,
                                                             .properties =
                                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                         });
        BufferContext::mapMemory(core, bufferContext);
    }

    void BufferObjectAbstract::lock(CommandPoolRef commandPool, const VkFence fence) {
        if (locked) {
            logger::log_err_silent("Double-call of BufferObjectAbstract::lock()");
            return;
        }

        VkBufferUsageFlags lockFlags = 0;

        switch (bufferLock) {
                using enum BufferLock;
            case LOCK_UNLOCK: lockFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                break;
            case LOCK_ONLY: lockFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                break;
            case ALWAYS_MUTABLE: {
                logger::log_err_silent("Cannot lock object because the given BufferLock is {}",
                                       static_cast<uint32_t>(bufferLock));
                return;
            }
        }

        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();


        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = hostDataObject->getTotalSizeByte(),
        };
        BufferContext::unmapMemory(core, bufferContext);
        MultiframeBufferContext lockedBuffer = BufferContext::createMultiframeContext(
            core, {
                .size = hostDataObject->getTotalSizeByte(),
                .usage = bufferUsage | lockFlags,
                .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            }
        );
        //NEW COMMAND BUFFER
        {
            const auto cex = ScopedNewCommandBufferExecutor(core, commandPool);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                BarrierUtils::cmdBufferMemoryBarrier(cex.getCommandBufferHandle(), VK_ACCESS_HOST_WRITE_BIT,
                                                     VK_ACCESS_TRANSFER_READ_BIT, bufferContext[i].buffer, 0,
                                                     hostDataObject->getTotalSizeByte(), VK_PIPELINE_STAGE_HOST_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), bufferContext[i].buffer, lockedBuffer[i].buffer, 1,
                                &copyRegion);
            }
        }
        if (fence == VK_NULL_HANDLE) {
            vkDeviceWaitIdle(device);
        } else {
            vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        }

        BufferContext::destroyContext(core, bufferContext);
        bufferContext = std::move(lockedBuffer);
        locked = true;
    }


    void BufferObjectAbstract::unlock(CommandPoolRef commandPool, const VkFence fence) {
        if (!locked) {
            logger::w_log_err_silent(L"Double-call of BufferObjectAbstract::unlock()");
            return;
        }
        VkBufferUsageFlags lockFlags = 0;

        switch (bufferLock) {
                using enum BufferLock;
            case LOCK_UNLOCK: lockFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                break;
            case LOCK_ONLY:
            case ALWAYS_MUTABLE: {
                logger::w_log_err_silent(L"Unlock is not allowed : BufferLock is not LOCK_UNLOCK");
                return;
            }
        }


        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();

        MultiframeBufferContext unlockedBuffer = BufferContext::createMultiframeContext(
            core, {
                .size = hostDataObject->getTotalSizeByte(),
                .usage = bufferUsage | lockFlags,
                .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            }
        );

        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = hostDataObject->getTotalSizeByte(),
        };
        //NEW COMMAND BUFFER
        {
            const auto cex = ScopedNewCommandBufferExecutor(core, commandPool);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                BarrierUtils::cmdBufferMemoryBarrier(cex.getCommandBufferHandle(), VK_ACCESS_SHADER_WRITE_BIT,
                                                     VK_ACCESS_TRANSFER_READ_BIT, bufferContext[i].buffer, 0,
                                                     hostDataObject->getTotalSizeByte(),
                                                     VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), bufferContext[i].buffer, unlockedBuffer[i].buffer, 1,
                                &copyRegion);
            }
        }
        if (fence == VK_NULL_HANDLE) {
            vkDeviceWaitIdle(device);
        } else {
            vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        }

        BufferContext::destroyContext(core, bufferContext);
        bufferContext = std::move(unlockedBuffer);
        locked = false;
    }


    void BufferObjectAbstract::destroy() {
        BufferContext::destroyContext(core, bufferContext);
    }
}
