//
// Created by Merutilm on 2025-07-10.
//

#include "BufferObject.hpp"

#include <iostream>

#include "../executor/ScopedCommandExecutor.hpp"
#include "../util/BufferImageUtils.hpp"
#include "../util/logger.hpp"

namespace merutilm::vkh {
    BufferObjectAbstract::BufferObjectAbstract(const CoreRef core, HostBufferObjectManager &&dataManager,
                               const VkBufferUsageFlags bufferUsage, const BufferLock bufferLock) : CoreHandler(core),
        hostBufferObject(std::make_unique<HostBufferObject>(std::move(dataManager))),
        bufferUsage(bufferUsage), bufferLock(bufferLock) {
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
        memcpy(bufferMapped[frameIndex], hostBufferObject->data.data(), hostBufferObject->getTotalSizeByte());
    }

    void BufferObjectAbstract::update(const uint32_t frameIndex, const uint32_t target) const {
        checkFinalizedBeforeUpdate();
        const uint32_t offset = hostBufferObject->getOffset(target);
        const uint32_t size = hostBufferObject->getSizeByte(target);
        memcpy(static_cast<std::byte *>(bufferMapped[frameIndex]) + offset, hostBufferObject->data.data() + offset, size);
    }

    void BufferObjectAbstract::checkFinalizedBeforeUpdate() const {
        if (locked) {
            throw exception_invalid_state(
                "BufferObjectAbstract::update() This bufferObject is already been finalized. It cannot be modified.");
        }
    }


    void BufferObjectAbstract::init() {
        const uint32_t size = hostBufferObject->getTotalSizeByte();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        buffers.resize(maxFramesInFlight);
        bufferMemory.resize(maxFramesInFlight);
        bufferMapped.resize(maxFramesInFlight);

        VkBufferUsageFlags lockFlags = 0;

        switch (bufferLock) {
                using enum BufferLock;
            case LOCK_UNLOCK:
            case LOCK_ONLY: lockFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                break;
            case ALWAYS_MUTABLE: lockFlags = 0;
                break;
        }

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            BufferImageUtils::initBuffer(core, size, bufferUsage | lockFlags,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                         &buffers[i], &bufferMemory[i]);
            vkMapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), bufferMemory[i], 0, size, 0,
                        &bufferMapped[i]);
        }
    }

    void BufferObjectAbstract::lock(CommandPoolRef commandPool) {
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
                logger::log_err_silent("Cannot lock object because the given BufferLock is {}", static_cast<uint32_t>(bufferLock));
                return;
            }
        }

        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        std::vector<VkBuffer> lockedBuffer(maxFramesInFlight);
        std::vector<VkDeviceMemory> lockedMemory(maxFramesInFlight);

        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = hostBufferObject->getTotalSizeByte(),
        }; {
            const auto cex = ScopedCommandExecutor(core, commandPool);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                vkUnmapMemory(device, bufferMemory[i]);
                BufferImageUtils::initBuffer(core, hostBufferObject->getTotalSizeByte(), bufferUsage | lockFlags,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                             &lockedBuffer[i], &lockedMemory[i]);
                VkBufferMemoryBarrier barrier = {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = buffers[i],
                    .offset = 0,
                    .size = hostBufferObject->getTotalSizeByte(),
                };

                vkCmdPipelineBarrier(cex.getCommandBufferHandle(),
                                     VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                     0, nullptr,
                                     1, &barrier,
                                     0, nullptr
                );
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), buffers[i], lockedBuffer[i], 1, &copyRegion);
            }
        }
        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkFreeMemory(device, bufferMemory[i], nullptr);
            vkDestroyBuffer(device, buffers[i], nullptr);
            buffers[i] = lockedBuffer[i];
            bufferMemory[i] = lockedMemory[i];
        }

        locked = true;
    }


    void BufferObjectAbstract::unlock(CommandPoolRef commandPool) {
        if (!locked) {
            logger::log_err_silent("Double-call of BufferObjectAbstract::unlock()");
            return;
        }
        VkBufferUsageFlags lockFlags = 0;

        switch (bufferLock) {
                using enum BufferLock;
            case LOCK_UNLOCK: lockFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                break;
            case LOCK_ONLY:
            case ALWAYS_MUTABLE: {
                logger::log_err_silent("Unlock is not allowed : BufferLock is not LOCK_UNLOCK");
                return;
            }
        }


        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        std::vector<VkBuffer> unlockedBuffer(maxFramesInFlight);
        std::vector<VkDeviceMemory> unlockedMemory(maxFramesInFlight);

        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = hostBufferObject->getTotalSizeByte(),
        }; {
            const auto cex = ScopedCommandExecutor(core, commandPool);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                BufferImageUtils::initBuffer(core, hostBufferObject->getTotalSizeByte(),
                                             bufferUsage | lockFlags,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                             &unlockedBuffer[i], &unlockedMemory[i]);

                VkBufferMemoryBarrier barrier = {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = buffers[i],
                    .offset = 0,
                    .size = hostBufferObject->getTotalSizeByte(),
                };

                vkCmdPipelineBarrier(cex.getCommandBufferHandle(),
                                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                     0, nullptr,
                                     1, &barrier,
                                     0, nullptr
                );
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), buffers[i], unlockedBuffer[i], 1, &copyRegion);
            }
        }
        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkFreeMemory(device, bufferMemory[i], nullptr);
            vkDestroyBuffer(device, buffers[i], nullptr);
            buffers[i] = unlockedBuffer[i];
            bufferMemory[i] = unlockedMemory[i];
            vkMapMemory(device, bufferMemory[i], 0, hostBufferObject->getTotalSizeByte(), 0, &bufferMapped[i]);
        }

        locked = false;
    }


    void BufferObjectAbstract::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            if (!locked) {
                vkUnmapMemory(device, bufferMemory[i]);
            }
            vkFreeMemory(device, bufferMemory[i], nullptr);
            vkDestroyBuffer(device, buffers[i], nullptr);
        }
    }
}
