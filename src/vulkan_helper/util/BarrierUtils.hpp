//
// Created by Merutilm on 2025-09-03.
//

#pragma once
#include "../core/vkh_base.hpp"

namespace merutilm::vkh {
    struct BarrierUtils {

    static void cmdMemoryBarrier(const VkCommandBuffer commandBuffer, const VkAccessFlags srcAccessMask,
                                 const VkAccessFlags dstAccessMask, const VkPipelineStageFlags srcStageMask,
                                 const VkPipelineStageFlags dstStageMask) {
        const VkMemoryBarrier memoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask
        };
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
    }


    static void cmdImageMemoryBarrier(const VkCommandBuffer commandBuffer, const VkImage image,
                                      const VkAccessFlags srcAccessMask,
                                      const VkAccessFlags dstAccessMask,
                                      const VkImageLayout oldLayout,
                                      const VkImageLayout newLayout,
                                      const uint32_t mipLevel,
                                      const VkPipelineStageFlags srcStageMask,
                                      const VkPipelineStageFlags dstStageMask) {
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
                .levelCount = 1,
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
    };
}
