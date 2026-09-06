//
// Created by Merutilm on 9/6/26.
//

#pragma once
#include <type_traits>
#include <vulkan_helper/core/Core.hpp>
#include <vulkan_helper/engine/context/BufferContext.hpp>
#include "../util/Utilities.h"

namespace merutilm::rff2 {

    enum class MemoryAllocationMode { NONE, NATIVE, VK_BUFFER };



    struct allocation_cancelled : std::runtime_error {
        explicit allocation_cancelled() : std::runtime_error("allocation cancelled") {}
    };

    template<typename Pod>
        requires std::is_trivially_copyable_v<Pod>
    struct CachedPodVector {


        vkh::Core &core;
        vkh::BufferContext ctx{};

        MemoryAllocationMode mode = MemoryAllocationMode::NONE;

        static constexpr uint64_t INITIAL_MAXIMUM_MEMORY = 17179869184;
        uint64_t allowedMaximumSize = INITIAL_MAXIMUM_MEMORY;
        Pod *raw = nullptr;
        size_t sizeUsed = 0;

#ifndef NDEBUG
        std::span<Pod> view{};
#endif

        explicit CachedPodVector(vkh::Core &core) : core(core) {
        }

        ~CachedPodVector() {
            if (mode == MemoryAllocationMode::NATIVE) {
                free(raw);
            }
            if (mode == MemoryAllocationMode::VK_BUFFER) {
                vkh::BufferContext::destroyContext(core, ctx);
            }
        }
        CachedPodVector(const CachedPodVector &) = delete;
        CachedPodVector operator=(const CachedPodVector &) = delete;
        CachedPodVector(CachedPodVector &&) = delete;
        CachedPodVector operator=(const CachedPodVector &&) = delete;

        void resizeWithWarning(const size_t newSize, const bool makeGpuReadable) {
            if (newSize > sizeUsed || newSize < sizeUsed / 4 + 1 || makeGpuReadable != (mode == MemoryAllocationMode::VK_BUFFER)) {

                const size_t calcedSize = newSize * sizeof(Pod);
                if (allowedMaximumSize < calcedSize &&
                    !vkh::logger::messagebox_yn(
                            "Warning", "The application has requested more than {} of memory. Do you want to continue?",
                            Utilities::formatByte(calcedSize))) {
                    throw allocation_cancelled();
                }

                allowedMaximumSize = std::max(allowedMaximumSize, calcedSize);
                if (mode == MemoryAllocationMode::NATIVE) {
                    free(raw);
                }
                if (mode == MemoryAllocationMode::VK_BUFFER) {
                    vkh::BufferContext::destroyContext(core, ctx);
                }
                if (newSize == 0) {
                    raw = nullptr;
                    ctx = {};
                    mode = MemoryAllocationMode::NONE;
                } else if (makeGpuReadable) {
                    ctx = vkh::BufferContext::createContext(core, {
                        .size = calcedSize,
                        .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                        .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT
                    });
                    vkh::BufferContext::mapMemory(core, ctx);
                    raw = reinterpret_cast<Pod *>(ctx.mappedMemory);
                    mode = MemoryAllocationMode::VK_BUFFER;
                } else {
                    raw = static_cast<Pod *>(malloc(calcedSize));
                    mode = MemoryAllocationMode::NATIVE;
                }
            }
#ifndef NDEBUG
            if (newSize == 0) {
                view = {};
            }else {
                std::ranges::fill_n(raw, newSize, Pod{});
                view = std::span(raw, newSize);
            }
#endif
            sizeUsed = newSize;
        }
    };
}