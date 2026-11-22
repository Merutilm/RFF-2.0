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
    [[nodiscard]] static MPAIndexMapper cGetFlattenIndexMapperFromCompressedPulledIndex(const MPAData<Num> &data, itt::ref compressedPulled, size_t levels);

    template<Number Num>
    [[nodiscard]] static MPAIndexMapper uGetFlattenIndexMapper(const MPAData<Num> &data, itt::ref iteration);

    template<Number Num>
    [[nodiscard]] static MPAIndexMapper cGetFlattenIndexMapper(const MPAData<Num> &data, itt::ref iteration);


    template<Number Num>
    [[nodiscard]] static std::span<const PA<Num>> getMPAFromMapper(const MPAData<Num> &data,
                                                                   MPAIndexMapper flattenIndexMapper);

    static void debugCheckMPAFromMapper(size_t totalSize, size_t mapped, size_t levels, size_t generatedLevels);

    // DEFINITION OF MPA UTILS
    // DEFINITION OF MPA UTILS
    // DEFINITION OF MPA UTILS



    template<Number Num>
    MPAIndexMapper cGetFlattenIndexMapperFromCompressedPulledIndex(const MPAData<Num> &data, const itt::ref compressedPulled, const size_t levels) {
        return MPAIndexMapper{.mapped = data.buffer->flattenIndexMapper.template interpret<MPAIndexMapper>()[compressedPulled].mapped,
                              .generatedLevels = levels};
    }

    template<Number Num>
    MPAIndexMapper uGetFlattenIndexMapper(const MPAData<Num> &data, const itt::ref iteration) {
        return data.buffer->flattenIndexMapper.template interpret<MPAIndexMapper>()[static_cast<size_t>(iteration)];
    }

    template<Number Num>
    MPAIndexMapper cGetFlattenIndexMapper(const MPAData<Num> &data, const itt::ref iteration) {
        const auto [pulled, levels] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(data.info, iteration);
        if (pulled == UINT64_MAX) {
            return MPAIndexMapper{.mapped = UINT64_MAX, .generatedLevels = 0};
        }
        return cGetFlattenIndexMapperFromCompressedPulledIndex(data, SRPCUtils::compress(data.info.pulledMPACompressor, pulled), levels);
    }

    template<Number Num>
    std::span<const PA<Num>> getMPAFromMapper(const MPAData<Num> &data, const MPAIndexMapper flattenIndexMapper) {

        const size_t levels = data.info.tablePeriods.size();
        size_t size = flattenIndexMapper.generatedLevels;
        debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, flattenIndexMapper.mapped, levels, size);
        PA<Num> *start = data.buffer->mpaTable.template interpret<PA<Num>>() + flattenIndexMapper.mapped;
        return std::span<const PA<Num>>(start, size);
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
