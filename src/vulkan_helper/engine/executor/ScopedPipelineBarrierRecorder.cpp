//
// Created by Merutilm on 8/29/26.
//

#include <vulkan_helper/engine/executor/ScopedPipelineBarrierRecorder.hpp>
namespace merutilm::vkh {

    ScopedPipelineBarrierRecorder::ScopedPipelineBarrierRecorder(const VkCommandBuffer recordingCommandBuffer,
                                                                 const VkDependencyFlags dependencyFlags) :
        recordingCommandBuffer(recordingCommandBuffer),
        dependencyFlags(dependencyFlags) {
        ScopedPipelineBarrierRecorder::init();
    }

    ScopedPipelineBarrierRecorder::~ScopedPipelineBarrierRecorder() { ScopedPipelineBarrierRecorder::cleanup(); }


    void ScopedPipelineBarrierRecorder::cmdBufferMemoryBarrier(const VkBufferMemoryBarrier &bufferMemoryBarrier,
                                                               const VkPipelineStageFlags srcStageMask,
                                                               const VkPipelineStageFlags dstStageMask) {
        bufferMemoryBarriers.emplace_back(bufferMemoryBarrier);
        unionFlags(srcStageMask, dstStageMask);
    }
    void ScopedPipelineBarrierRecorder::cmdMemoryBarrier(const VkMemoryBarrier &memoryBarrier,
                                                         const VkPipelineStageFlags srcStageMask,
                                                         const VkPipelineStageFlags dstStageMask) {
        memoryBarriers.emplace_back(memoryBarrier);
        unionFlags(srcStageMask, dstStageMask);
    }
    void ScopedPipelineBarrierRecorder::cmdImageMemoryBarrier(const VkImageMemoryBarrier &imageMemoryBarrier,
                                                              const VkPipelineStageFlags srcStageMask,
                                                              const VkPipelineStageFlags dstStageMask) {
        imageMemoryBarriers.emplace_back(imageMemoryBarrier);
        unionFlags(srcStageMask, dstStageMask);
    }
    void ScopedPipelineBarrierRecorder::unionFlags(const VkPipelineStageFlags srcStageMask,
                                                   const VkPipelineStageFlags dstStageMask) {
        this->srcStageMask |= srcStageMask;
        this->dstStageMask |= dstStageMask;
    }
    void ScopedPipelineBarrierRecorder::init() {
        // noop
    }
    void ScopedPipelineBarrierRecorder::cleanup() {
        vkCmdPipelineBarrier(recordingCommandBuffer, srcStageMask, dstStageMask,
                             dependencyFlags, static_cast<uint32_t>(memoryBarriers.size()),
                             memoryBarriers.size() == 0 ? nullptr : memoryBarriers.data(),
                             static_cast<uint32_t>(bufferMemoryBarriers.size()),
                             bufferMemoryBarriers.size() == 0 ? nullptr : bufferMemoryBarriers.data(),
                             static_cast<uint32_t>(imageMemoryBarriers.size()),
                             imageMemoryBarriers.size() == 0 ? nullptr : imageMemoryBarriers.data());
    }
} // namespace merutilm::vkh
