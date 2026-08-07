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
        explicit allocation_cancelled() : std::runtime_error("allocation cancelled") {}
    };

    template<Number Num>
    struct ApproxTableCache : ApproxTableCacheBase {
        using value_type = Num;

#ifndef NDEBUG
        std::vector<PA<Num>> mpaTable;
        std::vector<MPAIndexMapper> nonCompToPulledIndexMapper;
#else
        PA<Num> *mpaTable;
        MPAIndexMapper *nonCompToPulledIndexMapper;
#endif
        size_t tableSizeUsed;
        size_t mapperSizeUsed;

        explicit ApproxTableCache() = default;
        ~ApproxTableCache() override = default;
        ApproxTableCache(const ApproxTableCache &) = delete;
        ApproxTableCache &operator=(const ApproxTableCache &) = delete;
        ApproxTableCache(ApproxTableCache &&) = delete;
        ApproxTableCache &operator=(ApproxTableCache &&) = delete;

#ifndef NDEBUG
        template<typename Pod>
            requires std::is_trivially_copyable_v<Pod>
        void resizeWithWarning(std::vector<Pod> *container, const size_t oldSize, const size_t newSize) {
            if (newSize > oldSize || newSize < oldSize / 4) {
                const uint64_t size = newSize * sizeof(Pod);

                if (allowedMaximum < size &&
                    !vkh::logger::log_warn(
                            "The application has requested more than {} of memory. Do you want to continue?",
                            Utilities::formatByte(size))) {
                    throw allocation_cancelled();
                }

                allowedMaximum = std::max(allowedMaximum, size);
                container->resize(newSize);
            }
            std::ranges::fill_n(container->begin(), newSize, Pod{});
        }


        template<typename Pod>
            requires std::is_trivially_copyable_v<Pod>
        static void set(std::vector<Pod> &container, const size_t index, Pod val) {
            container[index] = val;
        }

#else
        template<typename Pod>
            requires std::is_trivially_copyable_v<Pod>
        void resizeWithWarning(Pod **container, const size_t oldSize, const size_t newSize) {
            if (newSize > oldSize || newSize < oldSize / 4) {
                const uint64_t size = newSize * sizeof(Pod);

                if (allowedMaximum < size &&
                    !vkh::logger::log_warn(
                            "The application has requested more than {} of memory. Do you want to continue?",
                            Utilities::formatByte(size))) {
                    throw allocation_cancelled();
                }

                allowedMaximum = std::max(allowedMaximum, size);
                free(*container);
                *container = static_cast<Pod *>(malloc(sizeof(Pod) * newSize));
            }
        }

        template<typename Pod>
            requires std::is_trivially_copyable_v<Pod>
        static void set(Pod *container, const size_t index, Pod val) {
            ::new (&container[index]) Pod(val);
        }

#endif


        void resize(const size_t tableLen, const size_t mapperLen) override {
            resizeWithWarning(&mpaTable, tableSizeUsed, tableLen);
            resizeWithWarning(&nonCompToPulledIndexMapper, mapperSizeUsed, mapperLen);
            tableSizeUsed = tableLen;
            mapperSizeUsed = mapperLen;
        }
    };

    using LightApproxTableCache = ApproxTableCache<double>;
    using DeepApproxTableCache = ApproxTableCache<dex>;

} // namespace merutilm::rff2
