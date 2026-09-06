//
// Created by Merutilm on 2025-05-23.
//

#pragma once

#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"
#include "CachedPodVector.hpp"
#include "vulkan_helper/base/vkh.hpp"

namespace merutilm::rff2 {

    struct ApproxTableCacheBase {

        virtual ~ApproxTableCacheBase() = default;
        virtual void resize(size_t tableLen, size_t mapperLen, bool makeGpuReadable) = 0;

        virtual size_t getTableSizeUsed() = 0;
        virtual size_t getMapperSizeUsed() = 0;
    };


    template<Number Num>
    struct ApproxTableCache : ApproxTableCacheBase {


        /**
         * flatten index table
         */
        CachedPodVector<PA<Num>> mpaTable;

        /**
         * for uncompressed table : iteration to flatten index
         * for compressed table : pulled compressed index to flatten index
         */
        CachedPodVector<MPAIndexMapper> flattenIndexMapper;

        explicit ApproxTableCache(vkh::Core &core) : mpaTable(core), flattenIndexMapper(core) {

        }

        ApproxTableCache(const ApproxTableCache &) = delete;
        ApproxTableCache &operator=(const ApproxTableCache &) = delete;
        ApproxTableCache(ApproxTableCache &&) = delete;
        ApproxTableCache &operator=(ApproxTableCache &&) = delete;

        size_t getTableSizeUsed() override {
            return mpaTable.sizeUsed;
        }

        size_t getMapperSizeUsed() override {
            return flattenIndexMapper.sizeUsed;
        }

        void resize(const size_t tableLen, const size_t mapperLen, const bool makeGpuReadable) override {
            mpaTable.resizeWithWarning(tableLen, makeGpuReadable);
            flattenIndexMapper.resizeWithWarning(mapperLen, makeGpuReadable);
        }
    };
} // namespace merutilm::rff2
