//
// Created by Merutilm on 2025-05-23.
//

#pragma once

#include "../algorithm/MPAIndexMapper.hpp"
#include "CachedPodVector.hpp"
#include "vulkan-helper/base/vkh.hpp"

namespace merutilm::rff2 {

    struct ApproxTableBuffer {

        /**
         * flatten index table
         */
        CachedPodVector mpaTable;

        /**
         * for uncompressed table : iteration to flatten index
         * for compressed table : pulled compressed index to flatten index
         */
        CachedPodVector flattenIndexMapper;

        explicit ApproxTableBuffer(vkh::Core &core) : mpaTable(core), flattenIndexMapper(core) {

        }

        ApproxTableBuffer(const ApproxTableBuffer &) = delete;
        ApproxTableBuffer &operator=(const ApproxTableBuffer &) = delete;
        ApproxTableBuffer(ApproxTableBuffer &&) = delete;
        ApproxTableBuffer &operator=(ApproxTableBuffer &&) = delete;

        template<typename PA> requires std::is_trivially_copyable_v<PA>
        void resize(const size_t tableLen, const size_t mapperLen, const bool makeGpuReadable) {
            mpaTable.resizeWithWarning<PA>(tableLen, makeGpuReadable);
            flattenIndexMapper.resizeWithWarning<MPAIndexMapper>(mapperLen, makeGpuReadable);
        }
    };
} // namespace merutilm::rff2
