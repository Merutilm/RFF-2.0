//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/util/BarrierUtils.hpp>
namespace merutilm::vkh {
    void BarrierUtils::cmdMemoryBarrier(const VkCommandBuffer commandBuffer,
        const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkPipelineStageFlags srcStageMask,
        const VkPipelineStageFlags dstStageMask) {
        const VkMemoryBarrier memoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask
        };
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 1, &memoryBarrier, 0, nullptr, 0,
                             nullptr);
    }

    void BarrierUtils::cmdBufferMemoryBarrier(const VkCommandBuffer commandBuffer,
        const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkBuffer buffer,
        const VkDeviceSize offset, const VkDeviceSize size, const VkPipelineStageFlags srcStageMask,
        const VkPipelineStageFlags dstStageMask) {
        const VkBufferMemoryBarrier bufferMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = buffer,
            .offset = offset,
            .size = size
        };
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0,
                             nullptr);
    }

    void BarrierUtils::cmdSynchronizeImageWriteToRead(const VkCommandBuffer commandBuffer,
        const VkImage image, const VkImageLayout currentLayout, const uint32_t mipLevel, const uint32_t mipLevelCount,
        const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask) {

        cmdImageMemoryBarrier(commandBuffer, image, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                              currentLayout, currentLayout, mipLevel, mipLevelCount, srcStageMask, dstStageMask);
    }

    void BarrierUtils::cmdImageMemoryBarrier(const VkCommandBuffer commandBuffer, const VkImage image,
        const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout oldLayout,
        const VkImageLayout newLayout, const uint32_t mipLevel, const uint32_t mipLevelCount,
        const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask) {
        const VkImageMemoryBarrier barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = mipLevel,
                .levelCount = mipLevelCount,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        vkCmdPipelineBarrier(commandBuffer, srcStageMask,
                             dstStageMask, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    }
}