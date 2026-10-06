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

    struct allocation_failed : std::runtime_error {
        explicit allocation_failed() : std::runtime_error("allocation failed") {}
    };
    struct allocation_cancelled : std::runtime_error {
        explicit allocation_cancelled() : std::runtime_error("allocation cancelled") {}
    };


    struct CachedPodVector {


        vkh::Core &core;
        vkh::BufferContext ctx{};

        MemoryAllocationMode mode = MemoryAllocationMode::NONE;

        static constexpr uint64_t INITIAL_MAXIMUM_MEMORY = 17179869184;
        uint64_t allowedMaximumSize = INITIAL_MAXIMUM_MEMORY;
        std::byte *raw = nullptr;
        size_t sizeUsed = 0;
        size_t allocated = 0;

        explicit CachedPodVector(vkh::Core &core) : core(core) {}

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

        template<typename Pod>
            requires std::is_trivially_copyable_v<Pod>
        void resizeWithWarning(const size_t newSize, const bool makeGpuReadable) {
            const size_t allocationSize = newSize * sizeof(Pod);
            if (allocationSize > allocated || allocationSize < allocated / 4 + 1 ||
                makeGpuReadable != (mode == MemoryAllocationMode::VK_BUFFER)) {

                if (allowedMaximumSize < allocationSize &&
                    !vkh::logger::messagebox_yn(
                            "Warning", "The application has requested more than {} of memory. Do you want to continue?",
                            Utilities::formatByte(allocationSize))) {
                    throw allocation_cancelled();
                }

                allowedMaximumSize = std::max(allowedMaximumSize, allocationSize);
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
                } else {
                    if (makeGpuReadable) {
                        ctx = vkh::BufferContext::createContext(core, {.size = allocationSize,
                                                                       .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                                       .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                                                                     VK_MEMORY_PROPERTY_HOST_CACHED_BIT});
                        vkh::BufferContext::mapMemory(core, ctx);
                        raw = ctx.mappedMemory;
                        mode = MemoryAllocationMode::VK_BUFFER;
                    } else {

                        raw = static_cast<std::byte *>(malloc(allocationSize));
                        mode = MemoryAllocationMode::NATIVE;
                    }

                    if (!raw) {
                        allocated = 0;
                        mode = MemoryAllocationMode::NONE;
                        vkh::logger::log("Memory allocation failed!!");
                        throw allocation_failed();
                    }
                }
                allocated = allocationSize;
            }
#ifndef NDEBUG
            if (raw != nullptr) std::ranges::fill_n(raw, allocationSize, static_cast<std::byte>(0));
#endif
            sizeUsed = newSize;
        }


        template<typename Pod>
            requires std::is_trivially_copyable_v<Pod>
        Pod * interpret() {
            return reinterpret_cast<Pod *>(raw);
        }
    };
} // namespace merutilm::rff2
