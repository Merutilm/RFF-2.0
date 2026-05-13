//
// Created by Merutilm on 2025-05-23.
//

#pragma once
#include <memory_resource>


#include "../mem/single_alloc_permitted_memory_resource.hpp"
#include "../mrthy/DeepPA.h"
#include "../mrthy/LightPA.h"

namespace merutilm::rff2 {

    template<typename PAB>
        requires std::is_base_of_v<PA, PAB>
    struct ApproxTableManager {

        std::unique_ptr<single_alloc_permitted_memory_resource> strictResource;
        std::unique_ptr<std::pmr::monotonic_buffer_resource>
                strictTablePool; // throws an exception when the capacity exceeds its size
        std::unique_ptr<std::pmr::vector<std::pmr::vector<PAB>>> mpaTable;

        explicit ApproxTableManager(size_t bufferSize, size_t tableWidth) {
            strictResource = std::make_unique<single_alloc_permitted_memory_resource>();
            strictTablePool = std::make_unique<std::pmr::monotonic_buffer_resource>(bufferSize, strictResource.get());
            mpaTable = std::make_unique<std::pmr::vector<std::pmr::vector<PAB>>>(
                    tableWidth, std::pmr::polymorphic_allocator(strictTablePool.get()));
        };

        ~ApproxTableManager() = default;
        ApproxTableManager(const ApproxTableManager &) = delete;
        ApproxTableManager &operator=(const ApproxTableManager &) = delete;
        ApproxTableManager(ApproxTableManager &&) = delete;
        ApproxTableManager &operator=(ApproxTableManager &&) = delete;
    };
} // namespace merutilm::rff2
