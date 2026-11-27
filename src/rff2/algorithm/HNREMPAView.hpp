//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../calc/iteration_types.hpp"
#include "../data/MPAData.hpp"
#include "HNREReferenceView.hpp"
#include "MPAUtils.hpp"
#include "PA.h"
#include "srpc_module/SRPCCursor.hpp"
namespace merutilm::rff2 {


    template<Number Num>
    class HNREMPAView : public SRPCCursor<itt::ref> {
        const HNREMPAData<Num> &data;
        size_t nextExpectedRefIteration = 0;
        size_t pulledTableSize;

        std::vector<uint64_t> itCount;
        std::vector<uint64_t> itCountLim;

    public:
        explicit HNREMPAView(const HNREMPAData<Num> &data);

        void reset() {
            SRPCCursor::reset();
            nextExpectedRefIteration = 1;
            itCount = data.info.tablePeriods;
            itCountLim = data.info.tablePeriods;
            std::ranges::fill(itCount, 0);
        }

        [[nodiscard]] const PA<Num> *lookup(HNREReferenceView<Num> &referenceView, itt::ref refIteration,
                                            const complex<Num> &dz) noexcept;


        void lookupBatch(std::array<PA<Num>, RFF_SIMD_CHUNK_SIZE> &results,
                         const std::array<itt::ref, RFF_SIMD_CHUNK_SIZE> &refIterations,
                         const std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> &dzs,
                         const std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &finishes) noexcept;

        [[nodiscard]] bool hasNthNext(size_t level) const;

    private:
        [[nodiscard]] const PA<Num> *uLookup(HNREReferenceView<Num> &referenceView, itt::ref refIteration,
                                             const complex<Num> &dz) noexcept;
        [[nodiscard]] const PA<Num> *cLookup(HNREReferenceView<Num> &referenceView, itt::ref refIteration,
                                             const complex<Num> &dz) noexcept;

        void nextPulledTableIndex();

        void jumpPulledTableIndex(size_t level);

        void jumpReference(HNREReferenceView<Num> &referenceView, itt::ref skip);

        void measureLevelCount(size_t startLevel);
    };

    template<Number Num>
    HNREMPAView<Num>::HNREMPAView(const HNREMPAData<Num> &data) : SRPCCursor(data.info.pulledMPACompressor), data(data) {
        pulledTableSize =
                MPAIndexMapperUtils::iterationToPulledTableIndex(data.info, data.info.tablePeriods.back() + 1);
        reset();
    }

    template<Number Num>
    const PA<Num> *HNREMPAView<Num>::lookup(HNREReferenceView<Num> &referenceView, const itt::ref refIteration,
                                        const complex<Num> &dz) noexcept {
        if (data.info.pulledMPACompressor.empty()) {
            return uLookup(referenceView, refIteration, dz);
        } else {
            return cLookup(referenceView, refIteration, dz);
        }
    }
    template<Number Num>
    void HNREMPAView<Num>::lookupBatch(std::array<PA<Num>, 16> &results, const std::array<itt::ref, 16> &refIterations,
                                   const std::array<complex<Num>, 16> &dzs,
                                   const std::array<uint8_t, 16> &finishes) noexcept {
        // TODO Change to AVX512 instructions
        // if (mpaPeriod == nullptr) {
        //     std::ranges::fill(results, PA<Num>::IDENTITY);
        //     return;
        // }
        //
        // std::array<MPAIndexMapper, RFF_SIMD_CHUNK_SIZE> mapperTemp;
        // std::array<Num, RFF_SIMD_CHUNK_SIZE> radiusTemp;
        // std::array<std::span<const PA<Num>>, RFF_SIMD_CHUNK_SIZE> tableTemp;
        //
        // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
        //     mapperTemp[i] = getFlattenIndexMapper(refIterations[i]);
        //     if (mapperTemp[i].mapped == UINT64_MAX) {
        //         mapperTemp[i].mapped = 0;
        //     } else
        //         MPAUtils::debugCheckMPAFromMapper(buffer.mpaTable.sizeUsed, mapperTemp[i].mapped,
        //                                 data.info.tablePeriods.size(), mapperTemp[i].generatedLevels);
        //     tableTemp[i] = getMPAFromMapper(mapperTemp[i]);
        //     radiusTemp[i] = dzs[i].max();
        // }
        //
        // switch (mpaSettings.selectionMethod) {
        //     using enum FrtMPASelectionMethod;
        //     case LOWEST: {
        //         for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
        //             const PA<Num> *pa = &PA<Num>::IDENTITY;
        //             if (!finishes[i]) {
        //                 for (const PA<Num> &test: tableTemp[i]) {
        //                     if (test.isValid(radiusTemp[i]))
        //                         pa = &test;
        //                     else
        //                         break;
        //                 }
        //             }
        //             results[i] = *pa;
        //         }
        //         break;
        //     }
        //     case HIGHEST: {
        //         for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
        //             const PA<Num> *pa = &PA<Num>::IDENTITY;
        //             if (!finishes[i] && !tableTemp[i].empty() && tableTemp[i].front().isValid(radiusTemp[i])) {
        //                 for (size_t j = tableTemp[i].size(); j > 0; --j) {
        //                     const PA<Num> &test = tableTemp[i][j - 1];
        //                     if (test.isValid(radiusTemp[i])) {
        //                         pa = &test;
        //                         break;
        //                     }
        //                 }
        //             }
        //             results[i] = *pa;
        //         }
        //         break;
        //     }
        //     default:
        //         break;
        // }
    }
    template<Number Num>
    bool HNREMPAView<Num>::hasNthNext(const size_t level) const {
        return originalIndex + data.info.skippableIterationCounts[level] < pulledTableSize;
    }


    template<Number Num>
    const PA<Num> *HNREMPAView<Num>::uLookup(HNREReferenceView<Num> &referenceView, const itt::ref refIteration,
                                         const complex<Num> &dz) noexcept {
        if (refIteration == 0 || !data.buffer) {
            return nullptr;
        }

        const MPAIndexMapper mapper = MPAUtils::uGetFlattenIndexMapper(data, refIteration);
        if (mapper.mapped == UINT64_MAX) {
            return nullptr;
        }

        MPAUtils::debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, mapper.mapped, data.info.tablePeriods.size(),
                                          mapper.generatedLevels);

        const auto table = MPAUtils::getMPAFromMapper<Num>(data, mapper);
        const Num r = dz.max();

        const PA<Num> *pa = nullptr;

        // This table cannot be empty because the pre-processing is done.
        pa = &table.front();

        if (pa->isValid(r)) {
            for (size_t j = table.size(); j > 1; --j) {
                if (const PA<Num> *test = &table[j - 1]; test->isValid(r)) {
                    pa = test;
                    break;
                }
            }
            referenceView.setIndex(refIteration + pa->skip); // O(1)
            return pa;
        }
        return nullptr;
    }

    template<Number Num>
    const PA<Num> *HNREMPAView<Num>::cLookup(HNREReferenceView<Num> &referenceView, const itt::ref refIteration,
                                         const complex<Num> &dz) noexcept {
        if (refIteration == 0) {
            reset();
            return nullptr;
        }
        if (!data.buffer || nextExpectedRefIteration != refIteration) {
            return nullptr;
        }


#ifndef NDEBUG
        auto [mapped, generatedLevels] =
                MPAIndexMapperUtils::iterationToPulledTableIndexMapper(data.info, refIteration);

        assert(mapped == originalIndex);
        const auto compressed = SRPCUtils::compress(data.info.pulledMPACompressor, originalIndex);
        assert(compressed == refIndex);
#endif
        const MPAIndexMapper mapper = MPAUtils::cGetFlattenIndexMapperFromCompressedPulledIndex(data, refIndex);
        assert(generatedLevels == mapper.generatedLevels);

        if (mapper.mapped == UINT64_MAX) {
            nextPulledTableIndex();
            return nullptr;
        }

        MPAUtils::debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, mapper.mapped, data.info.tablePeriods.size(),
                                          mapper.generatedLevels);

        const auto table = MPAUtils::getMPAFromMapper<Num>(data, mapper);
        const Num r = dz.max();

        const PA<Num> *pa = nullptr;

        // This table cannot be empty because the pre-processing is done.
        pa = &table.front();

        if (pa->isValid(r)) {
            for (size_t j = table.size(); j > 1; --j) {
                const size_t level = j - 1;
                const PA<Num> *test = &table[level];
                if (test->isValid(r)) {
                    pa = test;
                    if (hasNthNext(level)) {
                        jumpPulledTableIndex(level);
                    }
                    break;
                }
            }
            jumpReference(referenceView, pa->skip);
            return pa;
        }

        if (hasNthNext(0))
            nextPulledTableIndex();
        return nullptr;
    }

    template<Number Num>
    void HNREMPAView<Num>::nextPulledTableIndex() {
#ifndef NDEBUG
        assert(data.info.pulledMPACompressor.size() > 0);
        assert(itCount[0] == 0);
        assert(itCountLim[0] == data.info.tablePeriods[0]);
#endif

        nextExpectedRefIteration += data.info.tablePeriods[0];
        itCount[0] = data.info.tablePeriods[0]; // next pulled mpa sequence
        measureLevelCount(0);
        next(); // SRPCCursor::originalIndex is equal to pulledTableIndex
    }


    template<Number Num>
    void HNREMPAView<Num>::jumpPulledTableIndex(const size_t level) {

#ifndef NDEBUG
        for (size_t i = 0; i <= level; ++i) {
            assert(itCount[i] == 0);
            assert(itCountLim[i] == data.info.tablePeriods[i]);
        }
#endif
        nextExpectedRefIteration += data.info.tablePeriods[level];
        itCount[level] = data.info.tablePeriods[level]; // sequence jumping
        measureLevelCount(level);
        if (originalIndex == 0) {
            originalIndex = data.info.skippableIterationCounts[level];
            refIndex = data.info.compressedSkippableIterationCounts[level];
            startedUnitCount = data.info.compressionUnitCounts[level];
        } else
            jump(data.info.skippableIterationCounts[level]);
    }
    template<Number Num>
    void HNREMPAView<Num>::jumpReference(HNREReferenceView<Num> &referenceView, itt::ref skip) {
        if (skip == data.info.tablePeriods.back() - MPAUtils::PERTURBATION_REQ) {
            referenceView.endpoint();
        } else
            referenceView.jump(skip);
    }

    template<Number Num>
    void HNREMPAView<Num>::measureLevelCount(const size_t startLevel) {
        const auto &tablePeriods = data.info.tablePeriods;

        size_t level = startLevel;
        while (level < tablePeriods.size() - 1 && itCount[level] == itCountLim[level]) {
            itCount[level + 1] += itCount[level];
            ++level;
        }

        size_t levelCount = 0;
        while (level > 0) {
            --level;
            itCountLim[level] = std::min(tablePeriods[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            levelCount += (itCountLim[level] == tablePeriods[level]);
        }


        if (levelCount == 0) {
            assert(itCountLim[0] > 0);
            itCount[0] = itCountLim[0];
            nextExpectedRefIteration += itCountLim[0];
            measureLevelCount(0);
        }
    }

    template<Number Num>
    HNREMPAView<Num> HNREMPAData<Num>::makeMPAView() const {
        return HNREMPAView<Num>(*this);
    }

} // namespace merutilm::rff2
