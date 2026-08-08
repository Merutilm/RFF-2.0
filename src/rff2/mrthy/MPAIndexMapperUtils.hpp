//
// Created by Merutilm on 8/9/26.
//

#pragma once
#include "MPAPeriod.h"
namespace merutilm::rff2 {
    struct MPAIndexMapperUtils {

        enum class IndexMappingMode { FLATTEN_LEVELS, PULLED_LEVELS, FLATTEN, PULLED, LEVELS };

        template<IndexMappingMode MODE, typename Ret>
        static Ret invalidValueSentinel() {
            if constexpr (MODE == IndexMappingMode::LEVELS) {
                return 0;
            } else if constexpr (MODE == IndexMappingMode::PULLED_LEVELS || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                return MPAIndexMapper{UINT64_MAX, 0};
            } else if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::PULLED) {
                return UINT64_MAX;
            }
            throw std::logic_error("invalid mapping mode");
        }

        template<IndexMappingMode MODE, typename Ret>
        static Ret identityValueSentinel(const uint64_t levels) {
            if constexpr (MODE == IndexMappingMode::LEVELS) {
                return levels;
            } else if constexpr (MODE == IndexMappingMode::PULLED_LEVELS || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                return MPAIndexMapper{0, levels};
            } else if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::PULLED) {
                return 0;
            }
            throw std::logic_error("invalid mapping mode");
        }

        template<IndexMappingMode MODE, typename Ret>
        static Ret iterationToTableIndexMapper(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
            //
            // get index <=> Inverse calculation of index compression
            // First approach : check the remainder == 1
            //
            // [3, 11, 26]
            // 1 4 7 12 15 18 23 27 30 33 38
            // 3 1 1  2  1  1  2  3  1  1  2


            const auto &tablePeriod = mpaPeriod.tablePeriods;

            if (iteration == 0) {
                return invalidValueSentinel<MODE, Ret>();
            }
            if (iteration == 1) {
                return identityValueSentinel<MODE, Ret>(tablePeriod.size());
            }

            uint64_t index = 0;
            uint64_t levels = 0;
            uint64_t remainder = iteration;
            uint64_t maxSkip = UINT64_MAX;
            for (uint64_t i = tablePeriod.size(); i > 0; --i) {
                const uint64_t period = tablePeriod[i - 1];


                const uint64_t quotient = remainder / period;

                remainder -= quotient * period;
                maxSkip = std::min(period, maxSkip - quotient * period);
                if (remainder == 0 || maxSkip < tablePeriod[0])
                    return invalidValueSentinel<MODE, Ret>();

                levels += maxSkip >= period && remainder == 1;

                if constexpr (MODE == IndexMappingMode::PULLED || MODE == IndexMappingMode::PULLED_LEVELS) {
                    const uint64_t count = mpaPeriod.skippableIterationCounts[i - 1];
                    index += quotient * count;
                } else if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                    const uint64_t count = mpaPeriod.tableElementCounts[i - 1];
                    index += quotient * count + (maxSkip == period);
                }
            }

            if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                index -= levels;
            }

            if (remainder != 1 || maxSkip < tablePeriod[0])
                return invalidValueSentinel<MODE, Ret>();

            if constexpr (MODE == IndexMappingMode::PULLED) {
                return index;
            } else if constexpr (MODE == IndexMappingMode::PULLED_LEVELS) {
                return MPAIndexMapper{index, levels};
            } else if constexpr (MODE == IndexMappingMode::FLATTEN) {
                return index;
            } else if constexpr (MODE == IndexMappingMode::FLATTEN_LEVELS) {
                return MPAIndexMapper{index, levels};
            } else if constexpr (MODE == IndexMappingMode::LEVELS) {
                return levels;
            } else
                throw std::logic_error("invalid mapping mode");
        }

        static MPAIndexMapper iterationToFlattenTableIndexMapper(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::FLATTEN_LEVELS, MPAIndexMapper>(mpaPeriod, iteration);
        }

        static MPAIndexMapper iterationToPulledTableIndexMapper(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::PULLED_LEVELS, MPAIndexMapper>(mpaPeriod, iteration);
        }

        static uint64_t iterationToFlattenTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::FLATTEN, uint64_t>(mpaPeriod, iteration);
        }

        static uint64_t iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::PULLED, uint64_t>(mpaPeriod, iteration);
        }

        static uint64_t iterationToGeneratedLevels(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::LEVELS, uint64_t>(mpaPeriod, iteration);
        }
    };
} // namespace merutilm::rff2
