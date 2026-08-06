//
// Created by Merutilm on 2025-05-23.
//

#pragma once
#include <memory_resource>


#include "../calc/calculatable.hpp"
#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"
#include "../ui/Utilities.h"
#include "vulkan_helper/base/vkh.hpp"

namespace merutilm::rff2 {

    struct ApproxTableCacheBase {
        static constexpr uint64_t INITIAL_MAXIMUM_MEMORY = 17179869184;
        uint64_t allowedMaximum = INITIAL_MAXIMUM_MEMORY;

        virtual ~ApproxTableCacheBase() = default;

        virtual void resize(size_t tableLen, size_t mapperLen) = 0;
    };


    struct allocation_cancelled : std::runtime_error {
        explicit allocation_cancelled() : std::runtime_error("allocation denied") {}
    };

    template<Number Num>
    struct ApproxTableCache : ApproxTableCacheBase {
        using value_type = Num;

        std::vector<PA<Num>> mpaTable;
        std::vector<MPAIndexMapper> nonCompToPulledIndexMapper;
        uint64_t tableSizeUsed;

        explicit ApproxTableCache() = default;
        ~ApproxTableCache() override = default;
        ApproxTableCache(const ApproxTableCache &) = delete;
        ApproxTableCache &operator=(const ApproxTableCache &) = delete;
        ApproxTableCache(ApproxTableCache &&) = delete;
        ApproxTableCache &operator=(ApproxTableCache &&) = delete;


        template<typename Elem>
        void resizeWithWarning(std::vector<Elem> &container, size_t newSize) {
            if (newSize > container.size() || newSize < container.size() / 4) {
                const uint64_t size = newSize * sizeof(Elem);

                if (allowedMaximum < size &&
                    !vkh::logger::log_warn(
                            "The application has requested more than {} of memory. Do you want to continue?",
                            Utilities::formatByte(size))) {
                    throw allocation_cancelled();
                }

                allowedMaximum = std::max(allowedMaximum, size);
                container.resize(newSize);
            }
#ifndef NDEBUG
            std::ranges::fill_n(container.begin(), container.size(), Elem{});
#endif
        }


        void resize(const size_t tableLen, const size_t mapperLen) override {
            resizeWithWarning(mpaTable, tableLen);
            tableSizeUsed = tableLen;
            resizeWithWarning(nonCompToPulledIndexMapper, mapperLen);
        }
    };

    using LightApproxTableCache = ApproxTableCache<double>;
    using DeepApproxTableCache = ApproxTableCache<dex>;

} // namespace merutilm::rff2
