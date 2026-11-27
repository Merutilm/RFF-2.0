//
// Created by Merutilm on 11/15/26.
//

#pragma once
#include <cstdint>

#include "srpc_module/SRPCUtils.hpp"
#include "../calc/iteration_types.hpp"
#include "../data/MPAData.hpp"
#include "MPAIndexMapperUtils.hpp"
#include "PA.h"

namespace merutilm::rff2::MPAUtils {

    static constexpr int PERTURBATION_REQ = 2;

    template<Number Num>
    [[nodiscard]] static MPAIndexMapper cGetFlattenIndexMapperFromCompressedPulledIndex(const HNREMPAData<Num> &data, itt::ref compressedPulled);

    template<Number Num>
    [[nodiscard]] static MPAIndexMapper uGetFlattenIndexMapper(const HNREMPAData<Num> &data, itt::ref iteration);

    template<Number Num>
    [[nodiscard]] static MPAIndexMapper cGetFlattenIndexMapper(const HNREMPAData<Num> &data, itt::ref iteration);


    template<Number Num>
    [[nodiscard]] static std::span<const PA<Num>> getMPAFromMapper(const HNREMPAData<Num> &data,
                                                                   MPAIndexMapper flattenIndexMapper);

    static void debugCheckMPAFromMapper(size_t totalSize, size_t mapped, size_t levels, size_t generatedLevels);

    // DEFINITION OF MPA UTILS
    // DEFINITION OF MPA UTILS
    // DEFINITION OF MPA UTILS



    template<Number Num>
    MPAIndexMapper cGetFlattenIndexMapperFromCompressedPulledIndex(const HNREMPAData<Num> &data, const itt::ref compressedPulled) {
        return data.interpretMapper(compressedPulled);
    }

    template<Number Num>
    MPAIndexMapper uGetFlattenIndexMapper(const HNREMPAData<Num> &data, const itt::ref iteration) {
        return data.interpretMapper(static_cast<size_t>(iteration));
    }

    template<Number Num>
    MPAIndexMapper cGetFlattenIndexMapper(const HNREMPAData<Num> &data, const itt::ref iteration) {
        const itt::ref pulled = MPAIndexMapperUtils::iterationToPulledTableIndex(data.info, iteration);
        if (pulled == UINT64_MAX) {
            return MPAIndexMapper{.mapped = UINT64_MAX, .generatedLevels = 0};
        }
        return cGetFlattenIndexMapperFromCompressedPulledIndex(data, SRPCUtils::compress(data.info.pulledMPACompressor, pulled));
    }

    template<Number Num>
    std::span<const PA<Num>> getMPAFromMapper(const HNREMPAData<Num> &data, const MPAIndexMapper flattenIndexMapper) {

        const size_t levels = data.info.tablePeriods.size();
        size_t mapped = flattenIndexMapper.mapped;
        size_t size = flattenIndexMapper.generatedLevels;
        debugCheckMPAFromMapper(data.tableSize, mapped, levels, size);
        return data.interpretPA(mapped, size);
    }


    void debugCheckMPAFromMapper(const size_t totalSize, const size_t mapped, const size_t levels,
                                 const size_t generatedLevels) {
#ifndef NDEBUG
        if (levels == 0) {
            throw std::invalid_argument("levels is zero");
        }
        if (totalSize < mapped + generatedLevels) {
            throw std::invalid_argument("generatedLevels out of range");
        }
        if (levels < generatedLevels) {
            throw std::invalid_argument("levels out of range");
        }
#endif
    }


} // namespace merutilm::rff2::MPAUtils
