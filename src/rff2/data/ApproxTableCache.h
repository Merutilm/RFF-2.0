//
// Created by Merutilm on 2025-05-23.
//

#pragma once

#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"
#include "CachedPodVector.hpp"
#include "vulkan_helper/base/vkh.hpp"

namespace merutilm::rff2 {

    struct ApproxTableCache {


        /**
         * flatten index table
         */
        CachedPodVector mpaTable;

        /**
         * for uncompressed table : iteration to flatten index
         * for compressed table : pulled compressed index to flatten index
         */
        CachedPodVector flattenIndexMapper;

        explicit ApproxTableCache(vkh::Core &core) : mpaTable(core), flattenIndexMapper(core) {

        }

        ApproxTableCache(const ApproxTableCache &) = delete;
        ApproxTableCache &operator=(const ApproxTableCache &) = delete;
        ApproxTableCache(ApproxTableCache &&) = delete;
        ApproxTableCache &operator=(ApproxTableCache &&) = delete;

        template<typename PA> requires std::is_trivially_copyable_v<PA>
        void resize(const size_t tableLen, const size_t mapperLen, const bool makeGpuReadable) {
            mpaTable.resizeWithWarning<PA>(tableLen, makeGpuReadable);
            flattenIndexMapper.resizeWithWarning<MPAIndexMapper>(mapperLen, makeGpuReadable);
        }
    };
} // namespace merutilm::rff2
