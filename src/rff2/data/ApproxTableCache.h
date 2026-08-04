//
// Created by Merutilm on 2025-05-23.
//

#pragma once
#include <memory_resource>


#include "../calc/calculatable.hpp"
#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"

namespace merutilm::rff2 {

    struct ApproxTableCacheBase {
        virtual ~ApproxTableCacheBase() = default;

        virtual void resize(size_t tableLen, size_t mapperLen) = 0;
    };

    template<Number Num>
    struct ApproxTableCache : ApproxTableCacheBase {
        using value_type = Num;

        std::vector<PA<Num>> mpaTable;
        std::vector<MPAIndexMapper> nonCompToPulledIndexMapper;

        explicit ApproxTableCache() = default;
        ~ApproxTableCache() override = default;
        ApproxTableCache(const ApproxTableCache &) = delete;
        ApproxTableCache &operator=(const ApproxTableCache &) = delete;
        ApproxTableCache(ApproxTableCache &&) = delete;
        ApproxTableCache &operator=(ApproxTableCache &&) = delete;


        void resize(const size_t tableLen, const size_t mapperLen) override {
            if (tableLen > mpaTable.size() || tableLen < mpaTable.size() / 4) mpaTable.resize(tableLen);
            if (mapperLen > nonCompToPulledIndexMapper.size() || mapperLen < nonCompToPulledIndexMapper.size() / 4) nonCompToPulledIndexMapper.resize(mapperLen);
        }

    };

    using LightApproxTableCache = ApproxTableCache<double>;
    using DeepApproxTableCache = ApproxTableCache<dex>;

} // namespace merutilm::rff2
