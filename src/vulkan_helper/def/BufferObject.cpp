//
// Created by Merutilm on 2025-07-10.
//

#include "BufferObject.hpp"

#include <iostream>

#include "../executor/ScopedCommandExecutor.hpp"
#include "../util/BufferImageUtils.hpp"

namespace merutilm::vkh {
    BufferObject::BufferObject(const Core &core, std::unique_ptr<ShaderObjectManager> &&dataManager,
                               const VkBufferUsageFlags bufferUsage) : CoreHandler(core),
                                                                       CompleteShaderObjectManager(std::move(dataManager)),
                                                                       bufferUsage(bufferUsage) {
        BufferObject::init();
    }

    BufferObject::~BufferObject() {
        BufferObject::destroy();
    }


    void BufferObject::update(const uint32_t frameIndex) {
        checkFinalizedBeforeUpdate();

        if (!allInitialized) {
            for (uint32_t i = 0; i < initialized.size(); ++i) {
                if (!initialized[i]) {
                    throw exception_invalid_state(std::format("Buffer Object target {} is not initialized. Is called set()?", i));
                }
            }
            allInitialized = true;
        }
        memcpy(bufferMapped[frameIndex], data.data(), getTotalSizeByte());
    }

    void BufferObject::update(const uint32_t frameIndex, const uint32_t target) const {
        checkFinalizedBeforeUpdate();
        if (!initialized[target]) {
            throw exception_invalid_state(std::format("Buffer Object target {} is not initialized. Is called set()?", target));
        }
        const uint32_t offset = getOffset(target);
        const uint32_t size = getSizeByte(target);
        memcpy(static_cast<std::byte *>(bufferMapped[frameIndex]) + offset, data.data() + offset, size);
    }

    void BufferObject::checkFinalizedBeforeUpdate() const {
        if (finalized) {
            throw exception_invalid_state(
                "BufferObject::update() This bufferObject is already been finalized. It cannot be modified.");
        }
    }


    void BufferObject::init() {
        const uint32_t size = getTotalSizeByte();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        buffers.resize(maxFramesInFlight);
        bufferMemory.resize(maxFramesInFlight);
        bufferMapped.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            BufferImageUtils::initBuffer(core, size, bufferUsage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &buffers[i], &bufferMemory[i]);
            vkMapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), bufferMemory[i], 0, size, 0,
                        &bufferMapped[i]);
        }
    }

    void BufferObject::finalize(const CommandPool &commandPool) {
        if (finalized) {
            std::cerr << "Double-call of BufferObject::finalize()\n" << std::flush;
            return;
        }

        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        std::vector<VkBuffer> finalizedBuffer(maxFramesInFlight);
        std::vector<VkDeviceMemory> finalizedMemory(maxFramesInFlight);

        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = getTotalSizeByte(),
        }; {
            const auto cex = ScopedCommandExecutor(core, commandPool);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                vkUnmapMemory(device, bufferMemory[i]);
                BufferImageUtils::initBuffer(core, getTotalSizeByte(), bufferUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        &finalizedBuffer[i], &finalizedMemory[i]);
                VkBufferMemoryBarrier barrier = {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = buffers[i],
                    .offset = 0,
                    .size = getTotalSizeByte(),
                };

                vkCmdPipelineBarrier(cex.getCommandBufferHandle(),
                                     VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                     0, nullptr,
                                     1, &barrier,
                                     0, nullptr
                );
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), buffers[i], finalizedBuffer[i], 1, &copyRegion);
            }
        }
        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkFreeMemory(device, bufferMemory[i], nullptr);
            vkDestroyBuffer(device, buffers[i], nullptr);
            buffers[i] = finalizedBuffer[i];
            bufferMemory[i] = finalizedMemory[i];
        }

        finalized = true;
    }


    void BufferObject::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            if (!finalized) {
                vkUnmapMemory(device, bufferMemory[i]);
            }
            vkFreeMemory(device, bufferMemory[i], nullptr);
            vkDestroyBuffer(device, buffers[i], nullptr);
        }
    }
}
