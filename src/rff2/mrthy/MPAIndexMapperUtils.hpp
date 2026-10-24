//
// Created by Merutilm on 8/9/26.
//

#pragma once
#include "MPAPeriod.hpp"
namespace merutilm::rff2 {
    struct MPAIndexMapperUtils {

        enum class IndexMappingMode { FLATTEN_LEVELS, PULLED_LEVELS, FLATTEN, PULLED, LEVELS };

        template<IndexMappingMode MODE, typename Ret>
        static Ret invalidValueSentinel() {
            if constexpr (MODE == IndexMappingMode::LEVELS) {
                return 0;
            } else if constexpr (MODE == IndexMappingMode::PULLED_LEVELS || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                return MPAIndexMapper{.mapped = UINT64_MAX, .generatedLevels = 0};
            } else if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::PULLED) {
                return ~Ret(0); //max value
            }
            throw vkh::exception_invalid_state("invalid mapping mode");
        }

        template<IndexMappingMode MODE, typename Ret>
        static Ret identityValueSentinel(const size_t levels) {
            if constexpr (MODE == IndexMappingMode::LEVELS) {
                return levels;
            } else if constexpr (MODE == IndexMappingMode::PULLED_LEVELS || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                return MPAIndexMapper{.mapped = 0, .generatedLevels = levels};
            } else if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::PULLED) {
                return 0;
            }
            throw vkh::exception_invalid_state("invalid mapping mode");
        }

        template<IndexMappingMode MODE, typename Ret>
        static Ret iterationToTableIndexMapper(const MPAPeriod &mpaPeriod, const itt::ref iteration) {
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

            itt::ref index = 0;
            size_t levels = 0;
            itt::ref remainder = iteration;
            itt::ref maxSkip = ~static_cast<itt::ref>(0); //max value
            for (size_t i = tablePeriod.size(); i > 0; --i) {
                const itt::ref period = tablePeriod[i - 1];
                const itt::ref quotient = remainder / period;  //max quotient is 65535, but upcasted for trailing operation

                remainder -= quotient * period;
                maxSkip = std::min(period, maxSkip - quotient * period);

                if (remainder == 0 || maxSkip < tablePeriod[0])
                    return invalidValueSentinel<MODE, Ret>();

                levels += maxSkip == period && remainder == 1;

                if constexpr (MODE == IndexMappingMode::PULLED || MODE == IndexMappingMode::PULLED_LEVELS) {
                    const itt::ref count = mpaPeriod.skippableIterationCounts[i - 1];
                    index += quotient * count;
                } else if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                    const itt::ref count = mpaPeriod.tableElementCounts[i - 1];
                    index += quotient * count + (maxSkip == period);
                }
            }

            if constexpr (MODE == IndexMappingMode::FLATTEN || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                index -= levels;
            }

            if (remainder != 1 || maxSkip < tablePeriod[0])
                return invalidValueSentinel<MODE, Ret>();

            if constexpr (MODE == IndexMappingMode::PULLED || MODE == IndexMappingMode::FLATTEN) {
                // pulled permits 64-bit overflow (returns itt::ref), but flatten is not permitted because the flatten index
                // directly used for array indexer (returns size_t)
                return static_cast<Ret>(index);
            }
            else if constexpr (MODE == IndexMappingMode::PULLED_LEVELS || MODE == IndexMappingMode::FLATTEN_LEVELS) {
                //overflowing value is undefined
                return MPAIndexMapper{.mapped = static_cast<size_t>(index), .generatedLevels = levels};
            }
            else if constexpr (MODE == IndexMappingMode::LEVELS)
                return levels;
            else
                throw vkh::exception_invalid_state("invalid mapping mode");
        }

        static size_t iterationToNearestFlattenTableIndex(const MPAPeriod &mpaPeriod, const itt::ref iteration) {

            MPAIndexMapper mapper = invalidValueSentinel<IndexMappingMode::FLATTEN_LEVELS, MPAIndexMapper>();
            itt::ref iterationDecrement = 0;
            for (; mapper.mapped == UINT64_MAX; ++iterationDecrement) {
                mapper = iterationToFlattenTableIndexMapper(mpaPeriod, iteration - iterationDecrement);
                // TODO can this be optimized?
                // Even though the number of loops is less than 2 * MinSkipReference, optimization may still be
                // possible.
            }
            return iterationDecrement > 1 ? mapper.mapped + mapper.generatedLevels : mapper.mapped;
        }

        static MPAIndexMapper iterationToFlattenTableIndexMapper(const MPAPeriod &mpaPeriod, const itt::ref iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::FLATTEN_LEVELS, MPAIndexMapper>(mpaPeriod, iteration);
        }

        static MPAIndexMapper iterationToPulledTableIndexMapper(const MPAPeriod &mpaPeriod, const itt::ref iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::PULLED_LEVELS, MPAIndexMapper>(mpaPeriod, iteration);
        }

        static size_t iterationToFlattenTableIndex(const MPAPeriod &mpaPeriod, const itt::ref iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::FLATTEN, size_t>(mpaPeriod, iteration);
        }

        static itt::ref iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const itt::ref iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::PULLED, itt::ref>(mpaPeriod, iteration);
        }

        static size_t iterationToGeneratedLevels(const MPAPeriod &mpaPeriod, const itt::ref iteration) {
            return iterationToTableIndexMapper<IndexMappingMode::LEVELS, size_t>(mpaPeriod, iteration);
        }
    };
} // namespace merutilm::rff2
