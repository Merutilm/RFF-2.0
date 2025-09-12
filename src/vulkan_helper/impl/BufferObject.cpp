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
                                               const BufferLock bufferLock,
                                               const bool multiframeEnabled) : CoreHandler(core),
                                                                               hostDataObject(
                                                                                   factory::create<HostDataObject>(
                                                                                       std::move(dataManager))),
                                                                               bufferUsage(bufferUsage),
                                                                               bufferLock(bufferLock),
                                                                               multiframeEnabled(multiframeEnabled) {
        BufferObjectAbstract::init();
    }

    BufferObjectAbstract::~BufferObjectAbstract() {
        BufferObjectAbstract::destroy();
    }


    void BufferObjectAbstract::reloadBuffer() {
        BufferObjectAbstract::destroy();
        locked = false;
        BufferObjectAbstract::init();
    }

    void BufferObjectAbstract::update() const {
        checkFinalizedBeforeUpdate();
        memcpy(getBufferContext().mappedMemory, hostDataObject->data.data(), hostDataObject->getTotalSizeByte());
    }

    void BufferObjectAbstract::update(const uint32_t target) const {
        checkFinalizedBeforeUpdate();
        const uint32_t offset = hostDataObject->getOffset(target);
        const uint32_t size = hostDataObject->getSizeByte(target);
        memcpy(getBufferContext().mappedMemory + offset, hostDataObject->data.data() + offset, size);
    }

    void BufferObjectAbstract::updateMF(const uint32_t frameIndex) const {
        checkFinalizedBeforeUpdate();
        memcpy(getBufferContextMF(frameIndex).mappedMemory, hostDataObject->data.data(),
               hostDataObject->getTotalSizeByte());
    }

    void BufferObjectAbstract::updateMF(const uint32_t frameIndex, const uint32_t target) const {
        checkFinalizedBeforeUpdate();
        const uint32_t offset = hostDataObject->getOffset(target);
        const uint32_t size = hostDataObject->getSizeByte(target);
        memcpy(getBufferContextMF(frameIndex).mappedMemory + offset, hostDataObject->data.data() + offset, size);
    }

    void BufferObjectAbstract::checkFinalizedBeforeUpdate() const {
        if (locked) {
            throw exception_invalid_state(
                "BufferObjectAbstract::updateMF() This bufferObject is already been finalized. It cannot be modified.");
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

        const BufferInitInfo info{
            .size = size,
            .usage = bufferUsage | lockFlags,
            .properties =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        if (multiframeEnabled) {
            bufferContext = BufferContext::createMultiframeContext(core, info);
            BufferContext::mapMemory(core, getBufferContextMF());
        } else {
            bufferContext = BufferContext::createContext(core, info);
            BufferContext::mapMemory(core, getBufferContext());
        }
    }

    void BufferObjectAbstract::lock(CommandPoolRef commandPool, FencePtr const fence) {
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


        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = hostDataObject->getTotalSizeByte(),
        };
        const BufferInitInfo info{
            .size = hostDataObject->getTotalSizeByte(),
            .usage = bufferUsage | lockFlags,
            .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };

        if (multiframeEnabled) {
            MultiframeBufferContext lockedBuffer = BufferContext::createMultiframeContext(core, info);
            //NEW COMMAND BUFFER
            {
                const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
                BufferContext::unmapMemory(core, getBufferContextMF());
                const auto cex = ScopedNewCommandBufferExecutor(core, commandPool);

                for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                    BarrierUtils::cmdBufferMemoryBarrier(cex.getCommandBufferHandle(), VK_ACCESS_HOST_WRITE_BIT,
                                                         VK_ACCESS_TRANSFER_READ_BIT, getBufferContextMF(i).buffer, 0,
                                                         hostDataObject->getTotalSizeByte(), VK_PIPELINE_STAGE_HOST_BIT,
                                                         VK_PIPELINE_STAGE_TRANSFER_BIT);
                    vkCmdCopyBuffer(cex.getCommandBufferHandle(), getBufferContextMF(i).buffer, lockedBuffer[i].buffer,
                                    1,
                                    &copyRegion);
                }
            }

            if (fence == nullptr) {
                core.getLogicalDevice().waitDeviceIdle();
            } else {
                fence->wait();
            }

            BufferContext::destroyContext(core, getBufferContextMF());
            bufferContext = std::move(lockedBuffer);
        } else {
            BufferContext::unmapMemory(core, getBufferContext());
            BufferContext lockedBuffer = BufferContext::createContext(core, info);
            //NEW COMMAND BUFFER
            {
                const auto cex = ScopedNewCommandBufferExecutor(core, commandPool);
                BarrierUtils::cmdBufferMemoryBarrier(cex.getCommandBufferHandle(), VK_ACCESS_HOST_WRITE_BIT,
                                                     VK_ACCESS_TRANSFER_READ_BIT, getBufferContext().buffer, 0,
                                                     hostDataObject->getTotalSizeByte(), VK_PIPELINE_STAGE_HOST_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), getBufferContext().buffer, lockedBuffer.buffer, 1,
                                &copyRegion);
            }

            if (fence == nullptr) {
                core.getLogicalDevice().waitDeviceIdle();
            } else {
                fence->wait();
            }

            BufferContext::destroyContext(core, getBufferContext());
            bufferContext = std::move(lockedBuffer);
        }
        locked = true;
    }


    void BufferObjectAbstract::unlock(CommandPoolRef commandPool, FencePtr const fence) {
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


        const BufferInitInfo info {
            .size = hostDataObject->getTotalSizeByte(),
            .usage = bufferUsage | lockFlags,
            .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        const VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = hostDataObject->getTotalSizeByte(),
        };
        //NEW COMMAND BUFFER
        if (multiframeEnabled) {

            MultiframeBufferContext unlockedBuffer = BufferContext::createMultiframeContext(core, info);
            {
                const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
                const auto cex = ScopedNewCommandBufferExecutor(core, commandPool);
                for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                    BarrierUtils::cmdBufferMemoryBarrier(cex.getCommandBufferHandle(), VK_ACCESS_SHADER_WRITE_BIT,
                                                         VK_ACCESS_TRANSFER_READ_BIT, getBufferContextMF(i).buffer, 0,
                                                         hostDataObject->getTotalSizeByte(),
                                                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                                         VK_PIPELINE_STAGE_TRANSFER_BIT);
                    vkCmdCopyBuffer(cex.getCommandBufferHandle(), getBufferContextMF(i).buffer,
                                    unlockedBuffer[i].buffer, 1,
                                    &copyRegion);
                }
            }
            if (fence == VK_NULL_HANDLE) {
                core.getLogicalDevice().waitDeviceIdle();
            } else {
                fence->wait();
            }

            BufferContext::destroyContext(core, getBufferContextMF());
            bufferContext = std::move(unlockedBuffer);
            BufferContext::mapMemory(core, getBufferContextMF());
        } else {

            BufferContext unlockedBuffer = BufferContext::createContext(core, info);
            {
                const auto cex = ScopedNewCommandBufferExecutor(core, commandPool);
                BarrierUtils::cmdBufferMemoryBarrier(cex.getCommandBufferHandle(), VK_ACCESS_SHADER_WRITE_BIT,
                                                     VK_ACCESS_TRANSFER_READ_BIT, getBufferContext().buffer, 0,
                                                     hostDataObject->getTotalSizeByte(),
                                                     VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
                vkCmdCopyBuffer(cex.getCommandBufferHandle(), getBufferContext().buffer, unlockedBuffer.buffer, 1,
                                &copyRegion);
            }
            if (fence == VK_NULL_HANDLE) {
                core.getLogicalDevice().waitDeviceIdle();
            } else {
                fence->wait();
            }

            BufferContext::destroyContext(core, getBufferContext());
            bufferContext = std::move(unlockedBuffer);
            BufferContext::mapMemory(core, getBufferContext());
        }

        locked = false;
    }


    void BufferObjectAbstract::destroy() {
        if (multiframeEnabled) {
            BufferContext::destroyContext(core, getBufferContextMF());
        }else {
            BufferContext::destroyContext(core, getBufferContext());
        }
    }
}
