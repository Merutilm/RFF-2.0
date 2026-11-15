//
// Created by Merutilm on 11/15/26.
//

#pragma once
#include <cstdint>
#include <vector>

#include "../calc/iteration_types.hpp"
namespace merutilm::rff2::MPAUtils {

    static constexpr int PERTURBATION_REQ = 2;

    [[nodiscard]] static std::vector<uint64_t> uGenerateCurrentPASkips(const std::vector<itt::ref> &tablePeriod,
                                                                       const std::vector<uint64_t> &itCount);

    [[nodiscard]] static PulledMPACompressor
    generatePulledMPACompressor(const MPAInfo &info, const ReferenceCompressor &referenceCompressor);

    [[nodiscard]] static MPAIndexMapper cGetFlattenIndexMapperFromPulledIndex(const MPAData &data, itt::ref pulled, size_t levels);

    [[nodiscard]] static MPAIndexMapper uGetFlattenIndexMapper(const MPAData &data, itt::ref iteration);

    [[nodiscard]] static MPAIndexMapper cGetFlattenIndexMapper(const MPAData &data, itt::ref iteration);


    template<Number Num>
    [[nodiscard]] static std::span<const PA<Num>> getMPAFromMapper(const MPAData &data,
                                                                   MPAIndexMapper flattenIndexMapper);

    static void debugCheckMPAFromMapper(size_t totalSize, size_t mapped, size_t levels, size_t generatedLevels);

    // DEFINITION OF MPA UTILS
    // DEFINITION OF MPA UTILS
    // DEFINITION OF MPA UTILS

    static std::vector<uint64_t> uGenerateCurrentPASkips(const std::vector<itt::ref> &tablePeriod,
                                                         std::vector<uint64_t> &itCount) {
        const size_t levels = tablePeriod.size();
        std::vector<uint64_t> result = itCount;
        for (size_t j = 0; j < levels - 1; ++j) {
            if (result[j] >= static_cast<uint64_t>(tablePeriod[j]) - PERTURBATION_REQ) {
                result[j + 1] += result[j];
                result[j] = 0;
            }
        }
        return result;
    }


    PulledMPACompressor generatePulledMPACompressor(const MPAInfo &info,
                                                    const ReferenceCompressor &referenceCompressor) {
        PulledMPACompressor compressor;
        auto &tablePeriod = info.tablePeriods;
        auto &skippableIterationCounts = info.skippableIterationCounts;
        auto &isArtificial = info.isArtificial;

        for (const auto &unit: referenceCompressor) {
            const itt::ref start = unit.start;
            const itt::ref length = unit.range();
            const size_t index = ArrayUtils::binarySearch(tablePeriod, length + 1);

            // Check if the reference compressor is same as period.
            // However, The Computer doesn't know whether the compressor's length came from skipping to the periodic
            // point, or being cut off in the middle. So, Do check tableIndex too.

            if (const auto [pulledIndex, _] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(info, start);
                index != UINT64_MAX && pulledIndex != UINT64_MAX && !isArtificial[index]) {
                const itt::ref skippableIterationCount = skippableIterationCounts[index];
                compressor.emplace_back(pulledIndex + 1, pulledIndex + skippableIterationCount - 1);
            }
        }
        return compressor;
    }

    MPAIndexMapper cGetFlattenIndexMapperFromPulledIndex(const MPAData &data, const itt::ref pulled, const size_t levels) {
        const size_t comp = SRPCUtils::compress(data.compressor, pulled);
        return MPAIndexMapper{.mapped = data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[comp].mapped,
                              .generatedLevels = levels};
    }

    MPAIndexMapper uGetFlattenIndexMapper(const MPAData &data, const itt::ref iteration) {
        return data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[static_cast<size_t>(iteration)];
    }

    MPAIndexMapper cGetFlattenIndexMapper(const MPAData &data, const itt::ref iteration) {
        const auto [pulled, levels] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(data.info, iteration);
        if (pulled == UINT64_MAX) {
            return MPAIndexMapper{.mapped = UINT64_MAX, .generatedLevels = 0};
        }
        return cGetFlattenIndexMapperFromPulledIndex(data, pulled, levels);
    }

    template<Number Num>
    std::span<const PA<Num>> getMPAFromMapper(const MPAData &data, const MPAIndexMapper flattenIndexMapper) {

        const size_t levels = data.info.tablePeriods.size();
        size_t size = flattenIndexMapper.generatedLevels;
        debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, flattenIndexMapper.mapped, levels, size);
        PA<Num> *start = data.buffer->mpaTable.interpret<PA<Num>>() + flattenIndexMapper.mapped;
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
