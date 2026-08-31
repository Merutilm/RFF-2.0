//
// Created by Merutilm on 2025-09-03.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>

#include "vulkan_helper/engine/executor/ScopedPipelineBarrierRecorder.hpp"

namespace merutilm::vkh {
    struct BarrierUtils {


        static void cmdMemoryBarrier(VkCommandBuffer commandBuffer, VkAccessFlags srcAccessMask,
                                     VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask,
                                     VkPipelineStageFlags dstStageMask);

        static void cmdBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkAccessFlags srcAccessMask,
                                           VkAccessFlags dstAccessMask, VkBuffer buffer, VkDeviceSize offset,
                                           VkDeviceSize size, VkPipelineStageFlags srcStageMask,
                                           VkPipelineStageFlags dstStageMask);

        static void cmdSynchronizeImageWriteToRead(VkCommandBuffer commandBuffer, VkImage image,
                                                   VkImageLayout currentLayout, VkImageAspectFlags imageAspectMask,
                                                   uint32_t mipLevel, uint32_t mipLevelCount,
                                                   VkPipelineStageFlags srcStageMask,
                                                   VkPipelineStageFlags dstStageMask);
        static void cmdSynchronizeImageReadToWrite(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout,
                                            VkImageAspectFlags imageAspectMask, uint32_t mipLevel,
                                            uint32_t mipLevelCount, VkPipelineStageFlags srcStageMask,
                                            VkPipelineStageFlags dstStageMask);

        static void cmdOverlaySwapchain(VkCommandBuffer commandBuffer, VkImage swapchainImage);


        static void cmdImageMemoryBarrier(VkCommandBuffer commandBuffer, VkImage image, VkAccessFlags srcAccessMask,
                                          VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout,
                                          VkImageAspectFlags imageAspectMask, uint32_t mipLevel, uint32_t mipLevelCount,
                                          VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
    };
}
