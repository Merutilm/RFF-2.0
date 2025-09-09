//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "../impl/Core.hpp"
#include "../struct/BufferInitInfo.hpp"
#include "../util/BufferImageUtils.hpp"
#include "../core/allocator.hpp"

namespace merutilm::vkh {
    struct BufferContext;

    using MultiframeBufferContext = std::vector<BufferContext>;

    struct BufferContext {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
        VkDeviceSize bufferSize;
        std::byte *mappedMemory = nullptr;

        static BufferContext createContext(CoreRef core, const BufferInitInfo &bufferInitInfo) {
            BufferContext result = {};
            BufferImageUtils::initBuffer(core, bufferInitInfo, &result.buffer, &result.bufferMemory);
            result.bufferSize = bufferInitInfo.size;
            return result;
        }

        static MultiframeBufferContext createMultiframeContext(CoreRef core, const BufferInitInfo &bufferInitInfo) {
            const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
            std::vector<BufferContext> result(maxFramesInFlight);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                result[i] = createContext(core, bufferInitInfo);
            }

            return result;
        }

        static void mapMemory(CoreRef core, MultiframeBufferContext &context) {
            for (auto &ctx: context) {
                mapMemory(core, ctx);
            }
        }


        static void mapMemory(CoreRef core, BufferContext &context) {
            vkMapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), context.bufferMemory, 0, context.bufferSize,
                        0, reinterpret_cast<void **>(&context.mappedMemory));
        }

        static void unmapMemory(CoreRef core, const MultiframeBufferContext &context) {
            for (auto &ctx: context) {
                unmapMemory(core, ctx);
            }
        }

        static void unmapMemory(CoreRef core, const BufferContext &context) {
            vkUnmapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), context.bufferMemory);
        }

        template<typename T>
        static void fill(const BufferContext &bufCtx, const std::vector<T> &vec) {
            const size_t size = vec.size() * sizeof(T);
            if (size != bufCtx.bufferSize) {
                throw exception_invalid_args(std::format("Buffer size mismatch : {} and {}", size, bufCtx.bufferSize));
            }
            memcpy(bufCtx.mappedMemory, vec.data(), size);
        }

        static void flush(const VkDevice device, const BufferContext &bufCtx) {
            const VkMappedMemoryRange mappedMemoryRange = {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .pNext = nullptr,
                .memory = bufCtx.bufferMemory,
                .offset = 0,
                .size = bufCtx.bufferSize

            };
            vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
        }

        static void fillZero(const BufferContext &bufCtx) {
            std::fill_n(bufCtx.mappedMemory, bufCtx.bufferSize, static_cast<std::byte>(0));
        }

        template<typename T>
        static void set(const BufferContext &context, const uint32_t index, const T &value) {
            memcpy(context.mappedMemory + index * sizeof(T), &value, sizeof(T));
        }


        template<typename T>
        static T get(const BufferContext &context, const uint32_t index) {
            T value;
            memcpy(&value, context.mappedMemory + index * sizeof(T), sizeof(T));
            return value;
        }

        static std::vector<std::byte> getRaw(const BufferContext &context, const uint32_t offset, const uint32_t size) {
            std::vector<std::byte> value(size);
            memcpy(value.data(), context.mappedMemory + offset, size);
            return value;
        }

        static void destroyContext(CoreRef core, const MultiframeBufferContext &bufCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            for (const auto &[buffer, bufferMemory, bufferSize, mappedMemory]: bufCtx) {
                allocator::invoke(vkDestroyBuffer, device, buffer, nullptr);
                allocator::invoke(vkFreeMemory, device, bufferMemory, nullptr);
            }
        }


        static void destroyContext(CoreRef core, const BufferContext &bufCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            allocator::invoke(vkDestroyBuffer, device, bufCtx.buffer, nullptr);
            allocator::invoke(vkFreeMemory, device, bufCtx.bufferMemory, nullptr);
        }
    };
}
