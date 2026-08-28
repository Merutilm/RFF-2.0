//
// Created by Merutilm on 8/29/26.
//

#pragma once
#include <vulkan/vulkan.h>

#include "vulkan_helper/engine/cmd/CommandBuffer.hpp"

namespace merutilm::vkh {
    struct ScopedPipelineBarrierRecorder : Handler {

        VkCommandBuffer recordingCommandBuffer;
        VkPipelineStageFlags srcStageMask = 0;
        VkPipelineStageFlags dstStageMask = 0;
        VkDependencyFlags dependencyFlags = 0;

        std::vector<VkMemoryBarrier> memoryBarriers = {};
        std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers = {};
        std::vector<VkImageMemoryBarrier> imageMemoryBarriers = {};

        explicit ScopedPipelineBarrierRecorder(VkCommandBuffer recordingCommandBuffer,
                                               VkDependencyFlags dependencyFlags = 0);

        ~ScopedPipelineBarrierRecorder() override;

        ScopedPipelineBarrierRecorder(const ScopedPipelineBarrierRecorder &) = delete;

        ScopedPipelineBarrierRecorder &operator=(const ScopedPipelineBarrierRecorder &) = delete;

        ScopedPipelineBarrierRecorder(ScopedPipelineBarrierRecorder &&) = delete;

        ScopedPipelineBarrierRecorder &operator=(ScopedPipelineBarrierRecorder &&) = delete;


        void cmdMemoryBarrier(const VkAccessFlags srcAccessMask,
                              const VkAccessFlags dstAccessMask, const VkPipelineStageFlags srcStageMask,
                              const VkPipelineStageFlags dstStageMask) {
            const VkMemoryBarrier memoryBarrier = {.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
                                                   .pNext = nullptr,
                                                   .srcAccessMask = srcAccessMask,
                                                   .dstAccessMask = dstAccessMask};
            cmdMemoryBarrier(memoryBarrier, srcStageMask, dstStageMask);
        }

        void cmdBufferMemoryBarrier(const VkAccessFlags srcAccessMask,
                                    const VkAccessFlags dstAccessMask, const VkBuffer buffer, const VkDeviceSize offset,
                                    const VkDeviceSize size, const VkPipelineStageFlags srcStageMask,
                                    const VkPipelineStageFlags dstStageMask) {
            const VkBufferMemoryBarrier bufferMemoryBarrier = {.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                                                               .pNext = nullptr,
                                                               .srcAccessMask = srcAccessMask,
                                                               .dstAccessMask = dstAccessMask,
                                                               .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                               .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                               .buffer = buffer,
                                                               .offset = offset,
                                                               .size = size};
            cmdBufferMemoryBarrier(bufferMemoryBarrier, srcStageMask, dstStageMask);
        }



        void cmdImageMemoryBarrier(const VkImage image,
                                   const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
                                   const VkImageLayout oldLayout, const VkImageLayout newLayout,
                                   const VkImageAspectFlags imageAspectMask, const uint32_t mipLevel,
                                   const uint32_t mipLevelCount, const VkPipelineStageFlags srcStageMask,
                                   const VkPipelineStageFlags dstStageMask) {
            const VkImageMemoryBarrier imageMemoryBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                             .pNext = nullptr,
                                                             .srcAccessMask = srcAccessMask,
                                                             .dstAccessMask = dstAccessMask,
                                                             .oldLayout = oldLayout,
                                                             .newLayout = newLayout,
                                                             .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                             .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                             .image = image,
                                                             .subresourceRange = {.aspectMask = imageAspectMask,
                                                                                  .baseMipLevel = mipLevel,
                                                                                  .levelCount = mipLevelCount,
                                                                                  .baseArrayLayer = 0,
                                                                                  .layerCount = 1}};
            cmdImageMemoryBarrier(imageMemoryBarrier, srcStageMask, dstStageMask);
        }

        void cmdBufferMemoryBarrier(const VkBufferMemoryBarrier &bufferMemoryBarrier, VkPipelineStageFlags srcStageMask,
                                    VkPipelineStageFlags dstStageMask);
        void cmdMemoryBarrier(const VkMemoryBarrier &memoryBarrier, VkPipelineStageFlags srcStageMask,
                              VkPipelineStageFlags dstStageMask);
        void cmdImageMemoryBarrier(const VkImageMemoryBarrier &imageMemoryBarrier, VkPipelineStageFlags srcStageMask,
                                   VkPipelineStageFlags dstStageMask);

        void unionFlags(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);


    protected:
        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::vkh
