//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/util/BarrierUtils.hpp>

#include "vulkan_helper/engine/executor/ScopedPipelineBarrierRecorder.hpp"
namespace merutilm::vkh {



    void BarrierUtils::cmdMemoryBarrier(const VkCommandBuffer commandBuffer, const VkAccessFlags srcAccessMask,
                                        const VkAccessFlags dstAccessMask, const VkPipelineStageFlags srcStageMask,
                                        const VkPipelineStageFlags dstStageMask) {
        ScopedPipelineBarrierRecorder recorder(commandBuffer);
        recorder.cmdMemoryBarrier(srcAccessMask, dstAccessMask, srcStageMask, dstStageMask);
    }

    void BarrierUtils::cmdBufferMemoryBarrier(const VkCommandBuffer commandBuffer, const VkAccessFlags srcAccessMask,
                                              const VkAccessFlags dstAccessMask, const VkBuffer buffer,
                                              const VkDeviceSize offset, const VkDeviceSize size,
                                              const VkPipelineStageFlags srcStageMask,
                                              const VkPipelineStageFlags dstStageMask) {
        ScopedPipelineBarrierRecorder recorder(commandBuffer);
        recorder.cmdBufferMemoryBarrier(srcAccessMask, dstAccessMask, buffer, offset, size, srcStageMask, dstStageMask);
    }


    void BarrierUtils::cmdSynchronizeImageWriteToRead(const VkCommandBuffer commandBuffer, const VkImage image,
                                                      const VkImageLayout currentLayout, const VkImageAspectFlags imageAspectMask, const uint32_t mipLevel,
                                                      const uint32_t mipLevelCount,
                                                      const VkPipelineStageFlags srcStageMask,
                                                      const VkPipelineStageFlags dstStageMask) {
        ScopedPipelineBarrierRecorder recorder(commandBuffer);
        recorder.cmdImageMemoryBarrier(image, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                              currentLayout, currentLayout, imageAspectMask, mipLevel, mipLevelCount, srcStageMask, dstStageMask);
    }



    void BarrierUtils::cmdOverlaySwapchain(const VkCommandBuffer commandBuffer, const VkImage swapchainImage) {
        ScopedPipelineBarrierRecorder recorder(commandBuffer);
        recorder.cmdImageMemoryBarrier(swapchainImage, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                              VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
                              0, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }



    void BarrierUtils::cmdImageMemoryBarrier(const VkCommandBuffer commandBuffer, const VkImage image,
                                             const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
                                             const VkImageLayout oldLayout, const VkImageLayout newLayout,
                                             const VkImageAspectFlags imageAspectMask,
                                             const uint32_t mipLevel, const uint32_t mipLevelCount,
                                             const VkPipelineStageFlags srcStageMask,
                                             const VkPipelineStageFlags dstStageMask) {
        ScopedPipelineBarrierRecorder recorder(commandBuffer);
        recorder.cmdImageMemoryBarrier(image, srcAccessMask, dstAccessMask, oldLayout, newLayout, imageAspectMask, mipLevel, mipLevelCount, srcStageMask, dstStageMask);
    }
} // namespace merutilm::vkh
