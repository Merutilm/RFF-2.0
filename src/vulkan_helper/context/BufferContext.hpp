//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "../impl/Core.hpp"
#include "../struct/BufferInitInfo.hpp"
#include "../util/BufferImageUtils.hpp"

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
            for (auto &ctx : context) {
                mapMemory(core, ctx);
            }
        }


        static void mapMemory(CoreRef core, BufferContext &context) {
            vkMapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), context.bufferMemory, 0, context.bufferSize, 0, reinterpret_cast<void **>(&context.mappedMemory));
        }

        static void unmapMemory(CoreRef core, const MultiframeBufferContext &context) {
            for (auto &ctx : context) {
                unmapMemory(core, ctx);
            }
        }

        static void unmapMemory(CoreRef core, const BufferContext &context) {
            vkUnmapMemory(core.getLogicalDevice().getLogicalDeviceHandle(), context.bufferMemory);
        }



        static void destroyContext(CoreRef core, const MultiframeBufferContext & imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            for (const auto &[buffer, bufferMemory, bufferSize, mappedMemory]: imgCtx) {
                vkDestroyBuffer(device, buffer, nullptr);
                vkFreeMemory(device, bufferMemory, nullptr);
            }
        }


        static void destroyContext(CoreRef core, const BufferContext &bufCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            vkDestroyBuffer(device, bufCtx.buffer, nullptr);
            vkFreeMemory(device, bufCtx.bufferMemory, nullptr);
        }
    };
}
