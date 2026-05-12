//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/context/BufferContext.hpp>
#include <vulkan_helper/util/BufferImageUtils.hpp>
namespace merutilm::vkh {
    BufferContext BufferContext::createContext(Core & core, const BufferInitInfo &bufferInitInfo) {
        BufferContext result = {};
        BufferImageUtils::initBuffer(core, bufferInitInfo, &result.buffer, &result.bufferMemory);
        result.bufferSize = bufferInitInfo.size;
        return result;
    }

    MultiframeBufferContext BufferContext::createMultiframeContext(Core & core, const BufferInitInfo &bufferInitInfo) {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        std::vector<BufferContext> result(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            result[i] = createContext(core, bufferInitInfo);
        }

        return result;
    }

    void BufferContext::mapMemory(Core & core, MultiframeBufferContext &context) {
        for (auto &ctx: context) {
            mapMemory(core, ctx);
        }
    }

    void BufferContext::mapMemory(Core & core, BufferContext &context) {
        vkMapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), context.bufferMemory, 0, context.bufferSize,
                    0, reinterpret_cast<void **>(&context.mappedMemory));
    }

    void BufferContext::unmapMemory(Core & core, const MultiframeBufferContext &context) {
        for (auto &ctx: context) {
            unmapMemory(core, ctx);
        }
    }

    void BufferContext::unmapMemory(Core & core, const BufferContext &context) {
        vkUnmapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), context.bufferMemory);
    }


    void BufferContext::flush(const VkDevice device, const BufferContext &bufCtx) {
        const VkMappedMemoryRange mappedMemoryRange = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = bufCtx.bufferMemory,
            .offset = 0,
            .size = bufCtx.bufferSize

        };
        vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
    }

    void BufferContext::fillZero(const BufferContext &bufCtx) {
        std::fill_n(bufCtx.mappedMemory, bufCtx.bufferSize, static_cast<std::byte>(0));
    }


    std::vector<std::byte> BufferContext::getRaw(const BufferContext &context, const uint32_t offset,
        const uint32_t size) {
        std::vector<std::byte> value(size);
        memcpy(value.data(), context.mappedMemory + offset, size);
        return value;
    }

    void BufferContext::destroyContext(Core & core, const MultiframeBufferContext &bufCtx) {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        for (const auto &[buffer, bufferMemory, bufferSize, mappedMemory]: bufCtx) {
            vkDestroyBuffer(device, buffer, nullptr);
            vkFreeMemory(device, bufferMemory, nullptr);
        }
    }

    void BufferContext::destroyContext(Core & core, const BufferContext &bufCtx) {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkDestroyBuffer(device, bufCtx.buffer, nullptr);
        vkFreeMemory(device, bufCtx.bufferMemory, nullptr);
    }
}
