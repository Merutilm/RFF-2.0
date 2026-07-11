//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/core/Core.hpp>
#include <vulkan_helper/engine/wrapped/BufferInitInfo.hpp>

namespace merutilm::vkh {
    struct BufferContext;

    using MultiframeBufferContext = std::vector<BufferContext>;

    struct BufferContext {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
        VkDeviceSize bufferSize = 0;
        VkDeviceSize allocationSize = 0;
        std::byte *mappedMemory = nullptr;

        static BufferContext createContext(Core & core, const BufferInitInfo &bufferInitInfo);

        static MultiframeBufferContext createMultiframeContext(Core & core, const BufferInitInfo &bufferInitInfo);

        static void mapMemory(Core & core, MultiframeBufferContext &context);


        static void mapMemory(Core & core, BufferContext &context);

        static void unmapMemory(Core & core, const MultiframeBufferContext &context);

        static void unmapMemory(Core & core, const BufferContext &context);

        template<typename T>
        static void fill(const BufferContext &bufCtx, const std::vector<T> &vec);

        static void flush(VkDevice device, const BufferContext &bufCtx);

        static void fillZero(const BufferContext &bufCtx);

        template<typename T>
        static void set(const BufferContext &context, uint32_t index, const T &value);


        template<typename T>
        static T get(const BufferContext &context, uint32_t index);

        static std::vector<std::byte> getRaw(const BufferContext &context, uint32_t offset, uint32_t size);

        static void destroyContext(Core & core, const MultiframeBufferContext &bufCtx);


        static void destroyContext(Core & core, const BufferContext &bufCtx);
    };

    template<typename T>
    void BufferContext::fill(const BufferContext &bufCtx, const std::vector<T> &vec) {
        const size_t size = vec.size() * sizeof(T);
        if (size != bufCtx.bufferSize) {
            throw exception_invalid_args(std::format("Buffer size mismatch : {} and {}", size, bufCtx.bufferSize));
        }
        memcpy(bufCtx.mappedMemory, vec.data(), size);
    }
    template<typename T>
    void BufferContext::set(const BufferContext &context, const uint32_t index, const T &value) {
        memcpy(context.mappedMemory + index * sizeof(T), &value, sizeof(T));
    }

    template<typename T>
    T BufferContext::get(const BufferContext &context, const uint32_t index) {
        T value;
        memcpy(&value, context.mappedMemory + index * sizeof(T), sizeof(T));
        return value;
    }

}
